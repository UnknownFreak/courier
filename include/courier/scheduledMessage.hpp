#include <courier/message.hpp>
#include <courier/subscriberId.hpp>
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