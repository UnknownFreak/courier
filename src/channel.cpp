#include <courier/channel.hpp>
#include <courier/logger.hpp>

#include <algorithm>
#include <memory>
#include <string>

#include <omp.h>

namespace courier
{
	size_t g_channelId = 1;

	Channel::Channel() : Channel(1000000)
	{
	}

	Channel::Channel(const size_t reserveSize) : mMultithreadedEnabled(true), id((ChannelId)g_channelId++)
	{
		subscribers.reserve(reserveSize);
	}

	Channel::~Channel()
	{
		if (subscribers.size() != 0)
		{
			warning("Dangling subscribers on destruction");
			for (auto& sub : subscribers)
			{
				warning(std::to_string((size_t)sub.id));
			}
		}
	}
	size_t Channel::sendMessage(const Message& message)
	{
		if (mMultithreadedEnabled)
		{
#ifdef _WIN32
			int index;
#else
			size_t index;
#endif
			#pragma omp parallel for
			for (index = 0 ; index < subscribers.size(); index++)
			{
				subscribers[index].sendMessage(message);
			}
		}
		else
		{
			for (auto& s : subscribers)
			{
				s.sendMessage(message);
			}
		}
		return subscribers.size();
	}

	size_t Channel::sendMessage(const SubscriberId subscriberId, const Message& message)
	{
		Subscriber tmp;
		tmp.id = subscriberId;
		auto it = std::lower_bound(subscribers.begin(), subscribers.end(), tmp);
		if (it != subscribers.end())
		{
			it->sendMessage(message);
			return 1;
		}
		else
		{
			warning("Trying to send message to a subscriber that does not exist.");
		}
		return 0;
	}

	SubscriberId Channel::addSubscriber(const Subscriber& subscriber)
	{
		if (std::binary_search(subscribers.begin(), subscribers.end(), subscriber) == false)
		{
			subscribers.emplace_back(subscriber);
		}
		return subscriber.id;
	}

	void Channel::removeSubscriber(const SubscriberId subscriberId)
	{
		Subscriber tmp;
		tmp.id = subscriberId;
		auto it = std::lower_bound(subscribers.begin(), subscribers.end(), tmp);
		if (it != subscribers.end())
		{
			subscribers.erase(it);
		}
	}

	ChannelId Channel::getId() const
	{
		return id;
	}

	size_t Channel::getSubscribersCount() const
	{
		return subscribers.size();
	}

	void Channel::scheduleRemoval(const SubscriberId subscriberId)
	{
		scheduledRemovals.push_back(subscriberId);
	}

	void Channel::handleScheduledRemovals()
	{
		if (scheduledRemovals.size() > 0)
		{
			if (scheduledRemovals.size() < 10)
			{
				for (auto i : scheduledRemovals)
				{
					removeSubscriber(i);
				}
				scheduledRemovals.clear();
			}
			else
			{
				util::vectorFastRemove<Subscriber, SubscriberId>(subscribers, scheduledRemovals);
			}
		}
	}

	void Channel::setMultiThreaded(const bool bEnabled)
	{
		mMultithreadedEnabled = bEnabled;
	}

	void Channel::setChannelName(const std::string& inChannelName)
	{
		channelName = inChannelName;
	}

	const std::string& Channel::getChannelName() const
	{
		return channelName;
	}
}