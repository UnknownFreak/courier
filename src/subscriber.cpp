#include <courier/subscriber.hpp>
#include <courier/logger.hpp>

// reserve zero for un-initialized
size_t g_subscriberId = 1;

namespace courier
{

	Subscriber::Subscriber() : id(0),  m_ptr(), m_isAlive()
	{
	}

	Subscriber::Subscriber(const std::weak_ptr<bool>& isAlive, const std::function<void(const Message&)>& function) : id(g_subscriberId++), m_ptr(function), m_isAlive(isAlive)
	{
	}

	Subscriber::Subscriber(const Subscriber& sub) : id(sub.id), m_ptr(sub.m_ptr), m_isAlive(sub.m_isAlive)
	{
	}

	void Subscriber::sendMessage(const Message& message)
	{
		if (m_isAlive.expired() == false)
		{
			m_ptr(message);
		}
		else
		{
			warning("Trying to send a message to an object that is no longer alive.");
		}
	}

	Subscriber& Subscriber::operator=(const Subscriber& other)
	{
		if (this == &other)
		{
			return *this;
		}

		m_ptr = other.m_ptr;
		m_isAlive = other.m_isAlive;
		id = other.id;
		return *this;
	}

	bool Subscriber::operator < (const Subscriber& other) const
	{
		return id < other.id;
	}

	bool Subscriber::operator ==(const Subscriber& other) const
	{
		return id == other.id;
	}
}
