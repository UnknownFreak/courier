
#include "bench.hpp"
#include "courier/settings.hpp"

#include <chrono>

#include <courier/courier.hpp>
#include <iostream>

#include <courier/channels/objectChannel.hpp>

namespace bench
{

	enum class Topic
	{
		ExampleTopic,
		ExampleTopic2,
	};

	enum class MessageType
	{
		IntMessage,
	};


	constexpr courier::Topic convert(Topic t)
	{
		return courier::Topic(t);
	}

	//syntax sugaring with naming

	constexpr auto to = convert;
	constexpr auto withTopic = convert;
	constexpr auto from = convert;

	size_t g_id = 0;

	struct /*__attribute__((__aligned__(64)))*/ exampleSubscriber
	{
		// isAlive makes sure the subscriber target callback is valid before executing
		courier::SubscriberId id;

		int counter;
		int counter2;
		int bloat[20];

		exampleSubscriber() : id((courier::SubscriberId)g_id++), counter(0), counter2(0), bloat{ 0 }
		{
		}

		~exampleSubscriber() = default;

		inline courier::SubscriberId getId() const { return id; }

	};


	exampleSubscriber* setup2(auto& oc, size_t number)
	{
		exampleSubscriber* p = nullptr;
		for (size_t i = 0; i < number; i++)
		{
			p = &oc.emplace_back();
		}
		return p;
	}

	void run2(auto& oc, std::chrono::seconds dur)
	{
		auto start = std::chrono::high_resolution_clock::now();
		int counter = 0;
		//const char* const benchRun = "Bench Runner";
		ZoneNamed(benchRun, true);
		while (1)
		{
			oc.onMessage(to(::bench::Topic::ExampleTopic), courier::Message(::bench::MessageType::IntMessage, 1));
			counter++;
			if (counter == 10)
			{
				counter = 0;
				oc.onMessage(to(::bench::Topic::ExampleTopic2), courier::Message(::bench::MessageType::IntMessage, 1));

			}
			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> elapsed = end - start;
			if (elapsed >= dur)
				break;
		}
		FrameMark;
	}


	template<class T>
	using test2 = courier::ObjectChannel < T, decltype([](T& t, courier::Topic topic,  const courier::Message& msg) {
		switch ((size_t)topic) {
		case (size_t)to(::bench::Topic::ExampleTopic):
			t.counter += msg.get<int>();
			break;
		case (size_t)to(::bench::Topic::ExampleTopic2):
			t.counter2 += msg.get<int>();
			break;
		default:
			break;
		}
		}) > ;

	void runBench(size_t numSubscribers, std::chrono::seconds dur, size_t times = 4)
	{
		msgStat min{ 0,999999999999,0 };
		msgStat max{ 0,0,0 };
		msgStat avg{ 0,0,0 };

		auto oc = test2<exampleSubscriber>("exampleSubscriber");

		auto ex = setup2(oc, numSubscribers);

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
		std::cout << "Avg: " << avg.msgcount / times << " msg/s: " << avg.avgCount << std::endl;
	}

	std::string getCores(int cores)
	{
		if (cores > 0)
		{
			if (cores > omp_get_max_threads())
				return "cores = " + std::to_string(omp_get_max_threads());
			return "cores = " + std::to_string(cores);

		}
		return "cores = auto";
	}

	void run()
	{

		std::string input;
		int cores = -1;

		using namespace std::chrono_literals;

		std::chrono::duration d = 2s;
		size_t numItems = 1000000;
		size_t times = 3;

		std::cout << "Benchmark configuration" << std::endl;
		std::cout << "configure? (y/n)" << std::endl;
		std::getline(std::cin, input);

		if (input.length() > 0 && input[0] == 'y')
		{
			std::cout << "Num cores: (default=automatic)" << std::endl;
			std::getline(std::cin, input);
			cores = atoi(input.c_str());

			std::cout << "Num items: (default=1000000)" << std::endl;
			std::getline(std::cin,input);
			if (input.length() > 0)
			{
				numItems = strtoull(input.c_str(), nullptr, 10);
				if (numItems == 0)
				{
					numItems = 1000000;
				}
			}
			std::cout << "Num runs: (default=3)" << std::endl;
			std::getline(std::cin, input);
			if(input.length() > 0)
				times = strtoull(input.c_str(), nullptr, 10);
		}

		
		if (cores > 0)
		{
			courier::init(courier::Settings{ courier::ThreadingSettings::Fixed, cores });
		}
		else
		{
			courier::init(courier::Settings{ courier::ThreadingSettings::Auto });
		}
		{
			std::cout << "Benching courier "<< times <<" times, using " << getCores(cores) << " with " << numItems << " subscribers for " << d.count() << " seconds" << std::endl;

			runBench(numItems, d, times);
			std::cout << std::endl;
		}

		std::cout << std::endl;

		courier::shutdown();
	}
}