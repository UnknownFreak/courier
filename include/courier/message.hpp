#pragma once

#include <memory>

namespace courier
{
	class Message
	{
	public:

		constexpr Message();

		constexpr Message(const auto type)
			: msgType((unsigned int)type), m_arg()
		{
		}


		template<typename MsgType, class T>
		constexpr Message(const MsgType type, T val) : msgType((unsigned int)type), m_arg(std::make_shared<Arg<T>>(val))
		{
		}

		Message(const Message& other);
		Message& operator=(const Message& other);

		unsigned int msgType;

		constexpr inline bool is(const auto messageType) const { return msgType == (unsigned int)messageType; }
		
		template<typename T>
		constexpr inline T as() const { return (T)msgType; }

		template<class T>
		T get() const
		{
			if (m_arg.operator bool())
			{
				Arg<const T>* p = static_cast<Arg<const T>*>(m_arg.get());
				return p->t;
			}
			return T();
		}

	private:

		struct ArgBase
		{
			virtual ~ArgBase() = default;
		};
		template<class T>
		struct Arg : public ArgBase
		{
			constexpr Arg(): t() {}
			constexpr Arg(T arg) : t(arg) {}

			T t;
		};

		std::shared_ptr<ArgBase> m_arg;
	};
}