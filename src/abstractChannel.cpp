#include <courier/channel/abstractChannel.hpp>

namespace courier
{
	static size_t g_channelId = 1;

	AbstractChannel::AbstractChannel()
		: isMultiThreadedEnabled(true)
		, validator(nullptr)
		, channelId((ChannelId)g_channelId++)
		, channelName("")
	{
	}

	void AbstractChannel::setMultiThreaded(const bool bEnableMultiThreading)
	{
		isMultiThreadedEnabled = bEnableMultiThreading;
	}

	ChannelId AbstractChannel::getId() const
	{
		return channelId;
	}

	void AbstractChannel::setChannelName(std::string_view newChannelName)
	{
		channelName = newChannelName;
	}

	std::string_view AbstractChannel::getChannelName() const
	{
		return channelName;
	}

	void AbstractChannel::setMessageValidator(std::shared_ptr<MessageValidator> messageValidator)
	{
		validator = messageValidator;
	}

	std::shared_ptr<MessageValidator> AbstractChannel::getValidator() const
	{
		return validator;
	}

	bool AbstractChannel::validate(const Topic topic, const courier::Message& message) const
	{
		if (validator)
		{
			return validator->validate(topic, message);
		}
		return true;
	}
}