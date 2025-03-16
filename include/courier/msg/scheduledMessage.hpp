#pragma once

#include <courier/channelId.hpp>
#include <courier/msg/message.hpp>
#include <courier/subscriberId.hpp>

namespace courier::internal
{
	struct ScheduledMessage
	{
		const bool sendToChannel;
		const bool sendToSubscriber;
		const ChannelId channel;
		const SubscriberId subscriber;
		const Message message;
	};
}