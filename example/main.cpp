
#include <courier/courier.hpp>
#include <courier/logger.hpp>
#include <iostream>

namespace example
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

}

constexpr courier::Topic convert(example::Topic t)
{
	return courier::Topic(t);
}

//syntax sugaring with naming

constexpr auto to = convert;
constexpr auto withTopic = convert;
constexpr auto from = convert;

struct exampleSubscriber
{
	// isAlive makes sure the subscriber target callback is valid before executing
	std::shared_ptr<bool> isAlive;
	courier::SubscriberId subscriberId;

	exampleSubscriber() : isAlive(std::make_shared<bool>(true)), subscriberId(courier::SubscriberId::NOT_SET)
	{
		subscriberId = courier::get().addSubscriber(to(example::Topic::ExampleTopic), courier::Subscriber(isAlive, [](const courier::Message& msg) {

			if (msg.is(example::MessageType::FloatType))
			{
				std::cout << "Hello from subscriber: floatVal = " << msg.get<float>() << "\n";
			}
			else if (msg.is(example::MessageType::StringType))
			{
				std::cout << "Hello from subscriber: stringVal = " << msg.get<std::string>() << "\n";
			}
			}
		));
	}

	~exampleSubscriber()
	{
		courier::get().removeSubscriber(from(example::Topic::ExampleTopic), subscriberId);
		subscriberId = courier::SubscriberId::NOT_SET;
	}

};

// Validate a message before it sent to all subscribers, if the validation fails, the message is not sent
struct MyCustomMessageValidator : public courier::MessageValidator
{
	// Inherited via MessageValidator
	bool validate(const courier::Message& message) override
	{
		if(message.is(example::MessageType::StringType))
			return false;
		return true;
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

int main()
{
	std::cout << "Courier version: " << courier::getVersion() << std::endl;

	courier::init();
	courier::setLogger(std::make_shared<courierLogger>());
	auto& courier = courier::get();
	courier.createChannel(withTopic(example::Topic::ExampleTopic));

	// Message validator
	// courier.getChannel(withTopic(example::Topic::ExampleTopic))->setMessageValidator(std::make_shared<MyCustomMessageValidator>());

	{


		exampleSubscriber ex;

		// use std string implicitly to make sure the data is stored as std string and not anything else...
		courier.schedule(to(example::Topic::ExampleTopic), courier::Message(example::MessageType::StringType, std::string("A delayed scheduled message!")));

		int i = 0;
		while (i++ < 10)
		{
			courier.post(to(example::Topic::ExampleTopic), courier::Message(example::MessageType::FloatType, 10.f));
		}

		courier.handleScheduledMessages();

	}
	courier::shutdown();
	courier::clearLogger();

}