#include <courier/subscriber.hpp>
#include <courier/logger.hpp>

namespace courier
{

	Subscriber::Subscriber() : id(SubscriberId::NOT_SET)
	{
	}

	Subscriber::Subscriber(const SubscriberId inId) : id(inId)
	{
	}
}
