#include <courier/channel.hpp>

namespace courier
{
	static size_t g_channelId = 1;

	Channel::Channel()
		: validator(nullptr)
		, channelId((ChannelId)g_channelId++)
		, channelName("")
	{
	}

	ChannelId Channel::getId() const
	{
		return channelId;
	}

	void Channel::setChannelName(std::string_view newChannelName)
	{
		channelName = newChannelName;
	}

	std::string_view Channel::getChannelName() const
	{
		return channelName;
	}

	void Channel::setMessageValidator(std::shared_ptr<MessageValidator> messageValidator)
	{
		validator = messageValidator;
	}

	std::shared_ptr<MessageValidator> Channel::getValidator() const
	{
		return validator;
	}

	bool Channel::validate(const Topic topic, const courier::Message& message) const
	{
		if (validator)
		{
			return validator->validate(topic, message);
		}
		return true;
	}
}