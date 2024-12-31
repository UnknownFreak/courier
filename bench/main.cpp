
#include <courier/courier.hpp>
#include <iostream>

namespace example
{

	enum class Topic
	{
		ExampleTopic,
	};

	enum class MessageType
	{
		IntMessage,
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
	int counter;

	exampleSubscriber() : isAlive(std::make_shared<bool>(true)), subscriberId(courier::SubscriberId::NOT_SET), counter(0)
	{
	}

	exampleSubscriber(exampleSubscriber&& mv) noexcept : isAlive(nullptr), subscriberId(courier::SubscriberId::NOT_SET), counter(0)
	{
		isAlive = std::move(mv.isAlive);
		std::swap(mv.subscriberId, subscriberId);
		std::swap(mv.counter, counter);
	}

	~exampleSubscriber()
	{
		if (subscriberId != courier::SubscriberId::NOT_SET)
		{
			courier::get().scheduleRemoval(from(example::Topic::ExampleTopic), subscriberId);
			subscriberId = courier::SubscriberId::NOT_SET;
		}
	}

	void addSub()
	{
		subscriberId = courier::get().addSubscriber(to(example::Topic::ExampleTopic), courier::Subscriber(isAlive, [&](const courier::Message& ) {
			counter++;
		}
		));
	}

};

// Validate a message before it sent to all subscribers, if the validation fails, the message is not sent
struct MyCustomMessageValidator : public courier::MessageValidator
{
	size_t messageCount = 0;
	// Inherited via MessageValidator
	bool validate(const courier::Message& ) override
	{
		messageCount++;
		return true;
	}

	void clearMessageCounts()
	{
		messageCount = 0;
	}
};


std::vector<exampleSubscriber> vec;
auto validator = std::make_shared<MyCustomMessageValidator>();

void setup(size_t number)
{
	for (size_t i = 0; i < number; i++)
	{
		vec.emplace_back(exampleSubscriber{});
	}
	// temporary fix
	for(auto& e : vec)
	{
		e.addSub();
	}
}

void clear()
{
	vec.clear();
	courier::get().handleScheduledRemovals();
}

void run(std::chrono::seconds dur)
{
	auto& courier = courier::get();
	auto start = std::chrono::high_resolution_clock::now();

	while (1)
	{
		courier.post(to(example::Topic::ExampleTopic), courier::Message(example::MessageType::IntMessage, 1));

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> elapsed = end - start;
		if(elapsed >= dur)
			break;
	}
}

void bench(size_t numSubscribers, std::chrono::seconds dur)
{
	std::cout << "Benching courier with " << numSubscribers <<" subscribers for " << dur.count() << " seconds" << std::endl;
	setup(numSubscribers);
	run(dur);
	std::cout << "Message count during " << dur.count() << "s was: " << validator->messageCount / dur.count() << " messages/s" << std::endl;
	validator->clearMessageCounts();
	clear();
}

int main()
{
	std::cout << "Courier version: " << courier::getVersion() << std::endl;
	vec.reserve(500000);
	courier::init();
	auto& courier = courier::get();
	courier.createChannel(withTopic(example::Topic::ExampleTopic));

	courier.getChannel(withTopic(example::Topic::ExampleTopic))->setMessageValidator(validator);
	using namespace std::chrono_literals;

	std::cout << "test with openmp" << std::endl;
	{
		bench(10000, 1s);
		bench(100000, 1s);
		bench(500000, 1s);
		bench(500000, 5s);
	}
	std::cout << std::endl;

#ifdef _WIN32

	courier.getChannel(withTopic(example::Topic::ExampleTopic))->useOpenMp(false);
	std::cout << "test with ppl" << std::endl;
	{
		bench(10000, 1s);
		bench(100000, 1s);
		bench(500000, 1s);
		bench(500000, 5s);
	}

	system("pause");
#endif
	courier::shutdown();

}
