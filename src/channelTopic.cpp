#include <courier/channelTopic.hpp>
#include <courier/logger.hpp>

#ifdef _WIN32
#include <ppl.h>
#define _PPL 1
#else
#include <omp.h>
#define _PPL 0
#endif

namespace courier
{
	void ChannelTopic::addChannel(std::shared_ptr<Channel> channel)
	{
		channels.insert({ channel->getId(), channel });
	}

	bool ChannelTopic::removeChannel(const size_t channelId)
	{
		return channels.erase(channelId) == 1;
	}

	size_t ChannelTopic::sendMessage(const courier::Message& message)
	{
		if (validate(message) == false)
		{
			return 0;
		}
		size_t count = Channel::sendMessage(message);
		if (mMultithreadedEnabled)
		{
#if defined _PPL && _PPL == 1
			concurrency::parallel_for_each(channels.begin(), channels.end(),
				[&](auto& pair)
				{
					count += pair.second->sendMessage(message);
				});
#else
			//#pragma omp parallel for
			for (auto& channel : channels)
			{
				count += channel.second->sendMessage(message);
			}
#endif
		}
		else
		{
			for (auto& channel : channels)
			{
				count += channel.second->sendMessage(message);
			}
		}
		return count;
	}

	size_t ChannelTopic::sendMessage(const size_t subscriberId, const courier::Message& message)
	{
		if (validate(message) == false)
		{
			return 0;
		}
		return Channel::sendMessage(subscriberId, message);
	}


	size_t ChannelTopic::sendMessageCh(const size_t channelId, const courier::Message& message)
	{
		if (validate(message) == false)
		{
			return 0;
		}
		if (channels.find(channelId) != channels.end())
		{
			return channels[channelId]->sendMessage(message);
		}
		else
		{
			warning("Trying to post a message to a channel that does not exist");
		}
		return 0;
	}

	size_t ChannelTopic::sendMessage(const size_t channelId, const size_t subscriberId, const courier::Message& message)
	{
		if (validate(message) == false)
		{
			return 0;
		}
		if (channels.find(channelId) != channels.end())
		{
			return channels[channelId]->sendMessage(subscriberId, message);
		}
		else
		{
			warning("Trying to post a message to a channel that does not exist");
		}
		return 0;
	}

	void ChannelTopic::setMessageValidator(std::shared_ptr<MessageValidator> messageValidator)
	{
		validator = messageValidator;
	}

	std::shared_ptr<MessageValidator> ChannelTopic::getValidator() const
	{
		return validator;
	}


	bool ChannelTopic::validate(const courier::Message& message) const
	{
		if (validator)
		{
			return validator->validate(message);
		}
		return true;
	}
}