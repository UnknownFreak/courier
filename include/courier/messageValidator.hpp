#pragma once

#include <courier/message.hpp>

namespace courier
{
	class MessageValidator
	{
	public:

		inline virtual bool validate(const Message& message) = 0;
	};

}