#include "sample.hpp"

#include <iostream>

#include <courier/courier.hpp>
#include <courier/logger.hpp>

#include <courier/channels/objectChannel.hpp>

namespace sample
{

	enum class Topic
	{
		ExampleTopic,
	};

	enum class MessageType
	{
		FloatType,
		StringType
	};


	constexpr bool compare(courier::Topic t, Topic t2)
	{
		return t == courier::Topic(t2);
	}

	constexpr courier::Topic convert(Topic t)
	{
		return courier::Topic(t);
	}

	template<class T>
	constexpr T as(courier::Topic t)
	{
		return (T)t;
	}

	//syntax sugaring with naming

	constexpr auto is = compare;
	constexpr auto to = convert;
	constexpr auto withTopic = convert;
	constexpr auto from = convert;

	static size_t g_subId = 0;

	struct exampleSubscriber
	{

		courier::SubscriberId id;

		inline courier::SubscriberId getId() const { return id; }

		exampleSubscriber() : id((courier::SubscriberId)g_subId++)
		{
		}

		exampleSubscriber(const exampleSubscriber& other) : id(other.id) {}

		exampleSubscriber& operator=(const exampleSubscriber& other)
		{
			id = other.id;
			return *this;
		}

		virtual ~exampleSubscriber() = default;

		void onStringReceived(std::string& str)
		{
			std::cout << "Hello from subscriber: stringVal = " << str << "\n";
		}

		void onFloatReceived(const float flt)
		{
			std::cout << "Hello from subscriber: floatVal = " << flt << "\n";
		}
	};

	// Validate a message before it sent to all subscribers, if the validation fails, the message is not sent
	struct MyCustomMessageValidator : public courier::MessageValidator
	{
		// Inherited via MessageValidator
		bool validate(const courier::Topic topic, const courier::Message& message) override
		{
			if (compare(topic, Topic::ExampleTopic))
			{
				if (message.is(MessageType::StringType) == false)
					return true;
			}
			return false;
		}
	};

	struct courierLogger : public courier::Logger
	{
		// Inherited via Logger
		void info(const std::string_view v) override
		{
			std::cout << "Info: " << v << std::endl;
		}
		void warning(const std::string_view v) override
		{
			std::cout << "Warning: " << v << std::endl;

		}
		void error(const std::string_view v) override
		{
			std::cout << "Error: " << v << std::endl;
		}
	};

	void run()
	{

		courier::init(courier::Settings{ courier::ThreadingSettings::Auto });
		courier::setLogger(std::make_shared<sample::courierLogger>());
		auto& courier = courier::get();

		using ExampleChannel = courier::ObjectChannel < exampleSubscriber, decltype([]( exampleSubscriber& e, courier::Topic topic,const courier::Message& msg)
			{
				switch (as<sample::Topic>(topic))
				{
				case Topic::ExampleTopic:
					switch (msg.as<::sample::MessageType>())
					{
					case ::sample::MessageType::FloatType:
						e.onFloatReceived(msg.get<const float>());
						break;
					case ::sample::MessageType::StringType:
					{
						auto s = msg.get<std::string>();
						e.onStringReceived(s);
						break;
					}
					}
				default:
					break;
				}
			}) > ;

		auto channel = std::make_shared<ExampleChannel>("exampleChannel");
		courier.addChannel(channel);

		// Message validator
		// courier.getChannel(withTopic(example::Topic::ExampleTopic))->setMessageValidator(std::make_shared<MyCustomMessageValidator>());

		{
			std::vector<exampleSubscriber> objects;
			objects.push_back(exampleSubscriber());
			exampleSubscriber& ex = channel->emplace_back();


			// use std string implicitly to make sure the data is stored as std string and not anything else...
			courier.schedule(to(::sample::Topic::ExampleTopic), courier::Message(::sample::MessageType::StringType, std::string("A delayed scheduled message!")));

			int i = 0;
			while (i++ < 10)
			{
				courier.post(to(sample::Topic::ExampleTopic), courier::Message(sample::MessageType::FloatType, 10.f));
			}

			courier.handleScheduledMessages();

			channel->erase(ex.getId());

		}
		courier::shutdown();
		courier::clearLogger();
	}

}
