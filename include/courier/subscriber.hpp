#pragma once

#include <cstddef>

namespace courier
{
	enum class SubscriberId { NOT_SET };

	class Subscriber
	{
	public:

		Subscriber();
		Subscriber(const SubscriberId id);
		virtual ~Subscriber() = default;

		Subscriber(const Subscriber&) = default;
		Subscriber(Subscriber&&) = default;

		inline Subscriber& operator=(const Subscriber& other) = default;

		inline bool operator==(const Subscriber& other) const { return id == other.id; }
		inline bool operator<(const Subscriber& other) const { return (size_t)id < (size_t)other.id; }

		SubscriberId getId() const
		{
			return id;
		}

	private:

		SubscriberId id;

	};
}
