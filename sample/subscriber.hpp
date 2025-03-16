#pragma once

#include <courier/subscriberId.hpp>
#include <cstddef>

class Subscriber
{
public:

	Subscriber();
	Subscriber(const courier::SubscriberId id);
	virtual ~Subscriber() = default;

	Subscriber(const Subscriber&) = default;
	Subscriber(Subscriber&&) = default;

	inline Subscriber& operator=(const Subscriber& other) = default;

	courier::SubscriberId getId() const
	{
		return id;
	}

private:

	courier::SubscriberId id;

};
