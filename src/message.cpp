#include <courier/message.hpp>

namespace courier
{
	constexpr Message::Message() : msgType{}, m_arg()
	{
	}

	Message::Message(const Message& other) : msgType(other.msgType), m_arg(other.m_arg)
	{
	}

	Message& Message::operator=(const Message& other)
	{
		msgType = other.msgType;
		m_arg = other.m_arg;
		return *this;
	}
}
