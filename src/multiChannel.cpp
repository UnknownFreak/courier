#include <courier/multiChannel.hpp>
#include <courier/logger.hpp>

#include <omp.h>

namespace courier
{
	void MultiChannel::addChannel(std::shared_ptr<Channel> channel)
	{
		channels.insert({ channel->getId(), channel });
	}

	bool MultiChannel::removeChannel(const ChannelId channelId)
	{
		return channels.erase(channelId) == 1;
	}

	size_t MultiChannel::sendMessage(const courier::Message& message)
	{
		if (validate(message) == false)
		{
			return 0;
		}
		size_t count = Channel::sendMessage(message);
		if (mMultithreadedEnabled)
		{
			//TODO: can we parallel for a map, or do we have to change to vector...?			
			//#pragma omp parallel for
			for(auto it = channels.begin(); it != channels.end();  it++)
			{
				count += it->second->sendMessage(message);
			}
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

	size_t MultiChannel::sendMessage(const SubscriberId subscriberId, const courier::Message& message)
	{
		if (validate(message) == false)
		{
			return 0;
		}
		return Channel::sendMessage(subscriberId, message);
	}


	size_t MultiChannel::sendMessage(const ChannelId channelId, const courier::Message& message)
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

	size_t MultiChannel::sendMessage(const ChannelId channelId, const SubscriberId subscriberId, const courier::Message& message)
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

	void MultiChannel::setMessageValidator(std::shared_ptr<MessageValidator> messageValidator)
	{
		validator = messageValidator;
	}

	std::shared_ptr<MessageValidator> MultiChannel::getValidator() const
	{
		return validator;
	}


	bool MultiChannel::validate(const courier::Message& message) const
	{
		if (validator)
		{
			return validator->validate(message);
		}
		return true;
	}
}