
#include <courier/courier.hpp>
#include <iostream>

#include <courier/channel/objectChannel.hpp>
#include <courier/channel/objectChannel_fwdFunc.hpp>

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

size_t g_id = 0;

struct exampleSubscriber : public courier::Subscriber
{
	// isAlive makes sure the subscriber target callback is valid before executing
	int counter;

	exampleSubscriber() : Subscriber((courier::SubscriberId)g_id++), counter(0)
	{
	}

	~exampleSubscriber() = default;


	void onMessage(int) {
		counter++;
	}

};

exampleSubscriber* setup(auto& oc, size_t number)
{
	exampleSubscriber* p = nullptr;
	oc.setTopicCallback(to(example::Topic::ExampleTopic), [](exampleSubscriber& e, const courier::Message& msg)
		{
			auto i = msg.get<int>();
			e.onMessage(i);
		});
	for (size_t i = 0; i < number; i++)
	{
		p = &oc.emplace_back();
	}
	return p;
}

void run2(auto& oc, std::chrono::seconds dur)
{
	auto start = std::chrono::high_resolution_clock::now();

	while (1)
	{
		oc.onMessage(to(example::Topic::ExampleTopic), courier::Message(example::MessageType::IntMessage, 1));

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> elapsed = end - start;
		if (elapsed >= dur)
			break;
	}
}

struct msgStat
{
	long long dur = 0;
	long long msgcount = 999999999999;
	long long avgCount = 0;

	void set(long long d, long long msgC)
	{
		dur = d;
		msgcount = msgC;
		avgCount = msgcount / dur;
	}

	void add(long long d, long long msgC)
	{
		dur += d;
		msgcount += msgC;
		avgCount = msgcount / dur;
	}
};

void bench2(size_t numSubscribers, std::chrono::seconds dur, bool openmp=true, size_t times=4)
{
	msgStat min{ 0,999999999999,0 };
	msgStat max{ 0,0,0 };
	msgStat avg{ 0,0,0 };

	courier::ObjectChannel<exampleSubscriber> oc;

	oc.useOpenMp(openmp);
	auto ex = setup(oc, numSubscribers);
	
	std::cout << "Benching courier " << times << " times with " << numSubscribers << " subscribers for " << dur.count() << " seconds" << std::endl;
	for (size_t i = 0; i < times; i++)
	{
		run2(oc, dur);

		if (ex->counter < min.msgcount)
		{
			min.set(dur.count(), ex->counter);
		}

		if (ex->counter > max.msgcount)
		{
			max.set(dur.count(), ex->counter);
		}

		avg.add(dur.count(), ex->counter);

		ex->counter = 0;
	}

	std::cout << "Test results: " << std::endl;
	std::cout << "Max: " << max.msgcount << " msg/s: " << max.avgCount << std::endl;
	std::cout << "Min: " << min.msgcount << " msg/s: " << min.avgCount << std::endl;
	std::cout << "Avg: " << avg.msgcount << " msg/s: " << avg.avgCount << std::endl;
}



template<class T>
using test = courier::ObjectChannelFFunc < T, decltype([](T& t, const courier::Message& msg) { t.onMessage(msg.get<int>()); }) >;

void bench3(size_t numSubscribers, std::chrono::seconds dur, bool openmp = true, size_t times=4)
{
	msgStat min{ 0,999999999999,0 };
	msgStat max{ 0,0,0 };
	msgStat avg{ 0,0,0 };

	auto oc = test < exampleSubscriber>(to(example::Topic::ExampleTopic));

	oc.useOpenMp(openmp);
	auto ex = setup(oc, numSubscribers);

	std::cout << "Benching courier " << times << " times with " << numSubscribers << " subscribers for " << dur.count() << " seconds" << std::endl;
	for (size_t i = 0; i < times; i++)
	{
		run2(oc, dur);

		if (ex->counter < min.msgcount)
		{
			min.set(dur.count(), ex->counter);
		}

		if (ex->counter > max.msgcount)
		{
			max.set(dur.count(), ex->counter);
		}

		avg.add(dur.count(), ex->counter);

		ex->counter = 0;
	}

	std::cout << "Test results: " << std::endl;
	std::cout << "Max: " << max.msgcount << " msg/s: " << max.avgCount << std::endl;
	std::cout << "Min: " << min.msgcount << " msg/s: " << min.avgCount << std::endl;
	std::cout << "Avg: " << avg.msgcount << " msg/s: " << avg.avgCount << std::endl;
}

int main()
{
	std::cout << "Courier version: " << courier::getVersion() << std::endl;
	courier::init();
	using namespace std::chrono_literals;


#ifdef _WIN32
	{
		//std::cout << "run #" << i << std::endl;
		std::cout << "test rewrite forwardfunc with multithreading = ppl" << std::endl;
		bench3(500000, 5s, false);
		std::cout << std::endl;
	}

	{
		//std::cout << "run #" << i << std::endl;
		std::cout << "test rewrite with multithreading = ppl" << std::endl;
		bench2(500000, 5s, false);
		std::cout << std::endl;
	}
#endif

	{
		//std::cout << "run #" << i << std::endl;
		std::cout << "test rewrite forwardfunc with multithreading = openmp" << std::endl;
		bench3(500000, 5s);
		std::cout << std::endl;
	}
	{
		//std::cout << "run #" << i << std::endl;
		std::cout << "test rewrite with multithreading = openmp" << std::endl;
		bench2(500000, 5s);
		std::cout << std::endl;
	}


	std::cout << std::endl;

	courier::shutdown();
}
