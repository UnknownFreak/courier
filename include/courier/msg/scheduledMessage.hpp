#pragma once

#include <courier/msg/message.hpp>
#include <courier/subscriber.hpp>
#include <courier/channelId.hpp>

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