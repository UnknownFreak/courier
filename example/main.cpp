#include <iostream>

#include <courier/courier.hpp>
#include <courier/logger.hpp>

#include <courier/channel/objectChannel.hpp>

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

constexpr bool compare(courier::Topic t, example::Topic t2)
{
	return t == courier::Topic(t2);
}

constexpr courier::Topic convert(example::Topic t)
{
	return courier::Topic(t);
}

//syntax sugaring with naming

constexpr auto is = compare;
constexpr auto to = convert;
constexpr auto withTopic = convert;
constexpr auto from = convert;

static size_t g_subId = 0;

struct exampleSubscriber : public courier::Subscriber
{

	exampleSubscriber() : Subscriber((courier::SubscriberId)g_subId++)
	{
	}

	void onStringReceived(std::string& str)
	{
		std::cout << "Hello from subscriber: stringVal = " << str << "\n";
	}

	void onFloatReceived(const float flt)
	{
		std::cout << "Hello from subscriber: floatVal = " << flt << "\n";
	}
	using Subscriber::operator<;
};

// Validate a message before it sent to all subscribers, if the validation fails, the message is not sent
struct MyCustomMessageValidator : public courier::MessageValidator
{
	// Inherited via MessageValidator
	bool validate(const courier::Topic topic, const courier::Message& message) override
	{
		if (compare(topic, example::Topic::ExampleTopic))
		{
			if (message.is(example::MessageType::StringType) == false)
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

int main()
{
	std::cout << "Courier version: " << courier::getVersion() << std::endl;

	courier::init();
	courier::setLogger(std::make_shared<courierLogger>());
	auto& courier = courier::get();

	auto channel = std::make_shared<courier::ObjectChannel<exampleSubscriber>>();
	channel->setTopicCallback(to(example::Topic::ExampleTopic), [](exampleSubscriber & sub, const courier::Message & msg) {
		switch (msg.as<example::MessageType>())
		{
		case example::MessageType::FloatType:
			sub.onFloatReceived(msg.get<const float>());
		break;
		case example::MessageType::StringType:
		{
			auto s = msg.get<std::string>();
			sub.onStringReceived(s);
			break;
		}
		default:
			break;
		}
	});

	courier.addChannel(channel);

	// Message validator
	// courier.getChannel(withTopic(example::Topic::ExampleTopic))->setMessageValidator(std::make_shared<MyCustomMessageValidator>());

	{
		exampleSubscriber& ex = channel->emplace_back();


		// use std string implicitly to make sure the data is stored as std string and not anything else...
		courier.schedule(to(example::Topic::ExampleTopic), courier::Message(example::MessageType::StringType, std::string("A delayed scheduled message!")));

		int i = 0;
		while (i++ < 10)
		{
			courier.post(to(example::Topic::ExampleTopic), courier::Message(example::MessageType::FloatType, 10.f));
		}

		courier.handleScheduledMessages();

		channel->erase(ex.getId());

	}
	courier::shutdown();
	courier::clearLogger();

}