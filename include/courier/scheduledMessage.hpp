#include <courier/message.hpp>


namespace courier::internal
{
	struct ScheduledMessage
	{
		const bool sendToChannel;
		const bool sendToSubscriber;
		const size_t channel;
		const size_t subscriber;
		const Message message;
	};
}