#include "subscriber.hpp"

Subscriber::Subscriber() : id(courier::SubscriberId::NOT_SET)
{
}

Subscriber::Subscriber(const courier::SubscriberId inId) : id(inId)
{
}
