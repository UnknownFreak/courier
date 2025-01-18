#include <courier/channel.hpp>
#include <courier/logger.hpp>
#include <courier/util.hpp>

#ifdef _WIN32
#include <ppl.h>
#endif

#include <omp.h>
#include <algorithm>
#include <memory>
#include <string>

namespace courier
{
	size_t g_channelId = 1;

	Channel::Channel() : Channel(1000000)
	{
	}

	Channel::Channel(const size_t reserveSize) : mMultithreadedEnabled(true), mUseOpenMp(true), id((ChannelId)g_channelId++)
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
			if (mUseOpenMp)
			{
				int index;
				#pragma omp parallel for
				for (index = 0; index < subscribers.size(); index++)
				{
					subscribers[index].sendMessage(message);
				}
			}
			else
			{
				concurrency::parallel_for((size_t)0, subscribers.size(),
					[&](size_t index)
					{
						subscribers[index].sendMessage(message);
					});
			}
#else
			size_t index;
#pragma omp parallel for
			for (index = 0; index < subscribers.size(); index++)
			{
				subscribers[index].sendMessage(message);
			}
#endif

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

	void Channel::useOpenMp(const bool bEnabled)
	{
		mUseOpenMp = bEnabled;
	}

	void Channel::setChannelName(std::string_view inChannelName)
	{
		channelName = inChannelName;
	}

	std::string_view Channel::getChannelName() const
	{
		return channelName;
	}
}