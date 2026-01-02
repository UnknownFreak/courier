#include "courier/settings.hpp"
#include <courier/courier.hpp>
#include <courier/logger.hpp>

#include <omp.h>

courier::Courier* g_courier = nullptr;

namespace courier
{

	Courier::Courier(const Settings& in_settings) : settings(in_settings)
	{
		static int maxThreads = omp_get_max_threads();

		if (settings.threadSettings == courier::ThreadingSettings::Fixed)
		{
			int numThreads = maxThreads;
			if (settings.numThreads < maxThreads)
			{
				numThreads = settings.numThreads;
			}
			omp_set_num_threads(numThreads);
		}
	}

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

	void Courier::addChannel(std::shared_ptr<Channel> channel)
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
		mtx.lock();
		auto scheduledMessagesCopy = scheduledMessages;
		scheduledMessages.clear();
		mtx.unlock();
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

	size_t Courier::getScheduledMessageCount(const Topic topic)
	{
		size_t sizeVal = 0;
		mtx.lock();
		if (scheduledMessages.count(topic) != 0)
		{
			sizeVal = scheduledMessages[topic].size();
		}
		mtx.unlock();
		return sizeVal;
	}

	size_t Courier::getScheduledMessageCount()
	{
		size_t count = 0;
		mtx.lock();
		for (const auto& it : scheduledMessages)
		{
			count += it.second.size();
		}
		mtx.unlock();

		return count;
	}

	void init(const Settings& settings)
	{
		if (g_courier == nullptr)
		{
			info("Initializing Courier");
			g_courier = new Courier(settings);
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
