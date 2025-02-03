#include <courier/courier.hpp>
#include <courier/logger.hpp>

#include <format>

courier::Courier* g_courier = nullptr;

namespace courier
{
	void Courier::post(const Topic topic, const Message& message)
	{
		for (auto& channel : channels)
		{
			if(channel->validate(topic, message))
				channel->onMessage(topic, message);
		}
	}

	void Courier::post(const Topic topic, const SubscriberId subscriber, const Message& message)
	{
		for (auto& channel : channels)
		{
			if (channel->validate(topic, message))
				channel->onMessage(topic, subscriber, message);
		}
	}

	void Courier::post(const Topic topic, const ChannelId channelId, const Message& message)
	{
		for (auto& channel : channels)
		{
			if (channel->getId() == channelId)
			{
				if (channel->validate(topic, message))
					channel->onMessage(topic, message);
			}
		}
	}

	void Courier::post(const Topic topic, const ChannelId channelId, const SubscriberId subscriber, const Message& message)
	{
		for (auto& channel : channels)
		{
			if (channel->getId() == channelId)
			{
				if (channel->validate(topic, message))
					channel->onMessage(topic, subscriber, message);
			}
		}
	}
	void Courier::schedule(const Topic topic, const Message& message)
	{
		mtx.lock();
		{

			scheduledMessages[topic].push_back(internal::ScheduledMessage{
				false, false, ChannelId::NOT_SET, SubscriberId::NOT_SET, message
			});
		}
		mtx.unlock();
	}

	void Courier::schedule(const Topic topic, const SubscriberId subscriberId, const Message& message)
	{
		mtx.lock();
		{
			scheduledMessages[topic].push_back(internal::ScheduledMessage{ false, true, ChannelId::NOT_SET, subscriberId, message });
		}
		mtx.unlock();
	}

	void Courier::schedule(const Topic topic, const ChannelId channel, const Message& message)
	{
		mtx.lock();
		{
			scheduledMessages[topic].push_back(internal::ScheduledMessage{ false, true, channel, SubscriberId::NOT_SET, message });
		}
		mtx.unlock();
	}

	void Courier::schedule(const Topic topic, const ChannelId channel, const SubscriberId subscriber, const Message& message)
	{
		mtx.lock();
		{
			scheduledMessages[topic].push_back(internal::ScheduledMessage{
			true, true, channel, subscriber, message
				});
		}
		mtx.unlock();
	}

	void Courier::addChannel(std::shared_ptr<AbstractChannel> channel)
	{

		channels.push_back(channel);
	}

	bool Courier::removeChannel(const ChannelId channelId)
	{
		for (auto it = channels.begin(); it != channels.end(); it++)
		{
			if (it->get()->getId() == channelId)
			{
				channels.erase(it);
				return true;
			}
		}
		return false;
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
			channel->handleScheduledRemovals();
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
