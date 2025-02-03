#pragma once

#include <courier/msg/message.hpp>

#include <courier/topic.hpp>

namespace courier
{
	class MessageValidator
	{
	public:
		virtual ~MessageValidator() = default;
		inline virtual bool validate(const Topic topic, const Message& message) = 0;
	};

}