#include <courier/courier.hpp>
#include <courier/logger.hpp>

#include <format>

courier::Courier* g_courier = nullptr;

namespace courier
{
	void Courier::post(const Topic topic, const Message& message)
	{

		if (channels.find(topic) != channels.end())
		{
			m_messages += channels[topic]->sendMessage(message);
		}
		else
		{
			warning("Trying to post a message to a topic that has no registererd channel.");
		}
	}

	void Courier::post(const Topic topic, const size_t subscriber, const Message& message)
	{
		if (channels.find(topic) != channels.end())
		{
			m_messages += channels[topic]->sendMessage(subscriber, message);
		}
		else
		{
			warning("Trying to post a message to a topic that has no registererd channel.");
		}
	}

	void Courier::post(const Topic topic, const size_t channel, const size_t subscriber, const Message& message)
	{
		if (channels.find(topic) != channels.end())
		{
			if(subscriber == 0)
				m_messages += channels[topic]->sendMessage(channel, message);
			else
				m_messages += channels[topic]->sendMessage(channel, subscriber, message);
		}
		else
		{
			warning("Trying to post a message to a topic that has no registererd channel.");
		}
	}
	void Courier::schedule(const Topic topic, const Message& message)
	{
		mtx.lock();
		{

			scheduledMessages[topic].push_back(internal::ScheduledMessage{
				false, false, 0, 0, message
			});
		}
		mtx.unlock();
	}

	void Courier::schedule(const Topic topic, const size_t subscriberId, const Message& message)
	{
		mtx.lock();
		{
			scheduledMessages[topic].push_back(internal::ScheduledMessage{ false, true, 0, subscriberId, message });
		}
		mtx.unlock();
	}

	void Courier::schedule(const Topic topic, const size_t channel, const size_t subscriber, const Message& message)
	{
		mtx.lock();
		{
			scheduledMessages[topic].push_back(internal::ScheduledMessage{
			true, true, channel, subscriber, message
				});
		}
		mtx.unlock();
	}

	size_t Courier::addSubscriber(const Topic topic, const Subscriber& subscriber)
	{
		if (channels.find(topic) == channels.end())
		{
			info("Trying to add a subscriber to a topic that has no registered channel.");
			info("Creating channel automatically.");
			createChannel(topic);
		}
		return channels[topic]->addSubscriber(subscriber);
	}

	void Courier::removeSubscriber(const Topic topic, size_t subscriberId)
	{
		if (channels.find(topic) != channels.end())
		{
			channels[topic]->removeSubscriber(subscriberId);
		}
		else
		{
			warning("Trying to remove a subscriber from a topic that has no registererd channel.");
			warning(std::format("Topic = {}, subscriberId = {}", (unsigned int)topic, subscriberId));
		}
	}

	void Courier::createChannel(const Topic topic)
	{
		channels[topic] = std::make_shared<ChannelTopic>();
	}

	std::shared_ptr<ChannelTopic> Courier::getChannel(const Topic topic)
	{
		if (channels.find(topic) == channels.end())
		{
			createChannel(topic);
		}
		return channels[topic];
	}


	void Courier::scheduleRemoval(const Topic topic, const size_t subscriberId)
	{
		channels[topic]->scheduleRemoval(subscriberId);
	}

	void Courier::handleScheduledMessages()
	{
		auto scheduledMessagesCopy = scheduledMessages;
		scheduledMessages.clear();
		for (auto& [topic, messages] : scheduledMessagesCopy)
		{
			for (auto& message : messages)
			{
				if (message.sendToChannel == true)
				{
					if (message.sendToSubscriber)
					{
						post(topic, message.channel, message.subscriber, message.message);
					}
					else
					{
						post(topic, message.channel, message.message);
					}
				}
				else
				{
					if (message.sendToSubscriber)
					{
						post(topic, message.subscriber, message.message);
					}
					else
					{
						post(topic, message.message);
					}
				}
			}
		}
	}

	void Courier::handleScheduledRemovals()
	{
		for (auto& channel : channels)
		{
			channel.second->handleScheduledRemovals();
		}
	}

	size_t Courier::messageCount()
	{
		size_t tmp = m_messages;
		m_messages = 0;
		return tmp;
	}

	size_t Courier::getScheduledMessageCount(const Topic topic) const
	{
		if (scheduledMessages.count(topic) != 0)
		{
			return scheduledMessages.at(topic).size();
		}
		return 0;
	}

	size_t Courier::getScheduledMessageCount() const
	{
		size_t count = 0;
		for (const auto& it : scheduledMessages)
		{
			count += it.second.size();
		}
		return count;
	}

	void init()
	{
		if (g_courier == nullptr)
		{
			info("Initializing Courier");
			g_courier = new Courier();
		}
		else
		{
			warning("Trying to initialize Courier multiple times!");
		}
	}

	void shutdown()
	{
		delete g_courier;
		g_courier = nullptr;
	}

	Courier& get()
	{
		return *g_courier;
	}
}
