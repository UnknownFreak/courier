#include "courier/settings.hpp"
#include <courier/courier.hpp>
#include <courier/logger.hpp>

#include <omp.h>

//courier::Courier* g_courier = nullptr;

namespace courier
{
/*
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
		for (auto func : scheduledMessagesCopy)
		{
			func();
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

	size_t Courier::getScheduledMessageCount()
	{
		return scheduledMessages.size();
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
	*/
}
