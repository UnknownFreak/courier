
#include "stresstest.hpp"

#include <courier/courier.hpp>
#include <courier/settings.hpp>
#include <courier/messageHandler.hpp>

#include <cstdint>
#include <iostream>
#include <chrono>
#include <omp.h>
#include <ostream>


namespace stresstest
{
	struct Collection;
	static inline courier::Courier<Collection>* g_instance;

	//syntax sugaring with naming

	size_t g_id = 0;
	size_t g_id2 = 0;

	struct customType {};
	struct customType2 {};

	struct /*__attribute__((__aligned__(64)))*/ exampleSubscriber
	{
		// isAlive makes sure the subscriber target callback is valid before executing
		courier::ObjectId id;

		int counter;
		int counter2;
		//int bloat[20];

		exampleSubscriber() : id(), counter(0), counter2(0)//, bloat{ 0 }
		{
		}

		exampleSubscriber(const courier::ObjectId& inId) : id(inId), counter(0), counter2(0)//, bloat{ 0 }
		{
		}

		~exampleSubscriber() = default;

		inline courier::ObjectId getId() const { return id; }

	};

	struct /*__attribute__((__aligned__(64)))*/ exampleSubscriber2
	{
		// isAlive makes sure the subscriber target callback is valid before executing
		courier::ObjectId id;

		int counter;
		int counter2;
		float x=0,y=0,z=0;

		exampleSubscriber2() : id(), counter(0), counter2(0)
		{
		}
		exampleSubscriber2(const courier::ObjectId& inId) : id(inId), counter(0), counter2(0)//, bloat{ 0 }
		{
		}

		~exampleSubscriber2() = default;

		inline courier::ObjectId getId() const { return id; }

	};

};

namespace courier
{
	template<>
	[[using gnu: hot, flatten]] void handleObjectMessage(stresstest::exampleSubscriber& ex, const int)
	{
		ex.counter++;
	}

	template<>
	[[using gnu: hot, flatten]] void handleObjectMessage(stresstest::exampleSubscriber& ex, const stresstest::customType)
	{
		ex.counter2++;
	}
	
	template<>
	[[using gnu: hot, flatten]] void handleObjectMessage(stresstest::exampleSubscriber&, const stresstest::customType2)
	{
	}

	template<>
	[[using gnu: hot, flatten]] void handleObjectMessage(stresstest::exampleSubscriber2& ex, const int)
	{
		ex.counter++;
	}
	template<>
	[[using gnu: hot, flatten]] void handleObjectMessage(stresstest::exampleSubscriber2& ex, const stresstest::customType)
	{
		ex.counter2++;
	}
}

namespace stresstest
{
	struct Collection
	{
		std::vector<exampleSubscriber> collection;
		std::vector<exampleSubscriber2> collection2;
		std::vector<exampleSubscriber2> collection3;
		std::vector<exampleSubscriber2> collection4;

		template<class Type>
			requires(sizeof(Type) >= sizeof(uintptr_t))
		void onMessage(const Type& message)
		{
			courier::handleMessage(collection, message);
			courier::handleMessage(collection2, message);
			courier::handleMessage(collection3, message);
			courier::handleMessage(collection4, message);
		}

		template<class Type>
			requires(sizeof(Type) < sizeof(uintptr_t))
		void onMessage(const Type message)
		{
			courier::handleMessage(collection, message);
			courier::handleMessage(collection2, message);
			courier::handleMessage(collection3, message);
			courier::handleMessage(collection4, message);
		}

		template<class Type>
		void onMessage(courier::ObjectId id, const Type& message)
		{
			courier::handleMessage(collection, id, message);
			courier::handleMessage(collection2, id, message);
			courier::handleMessage(collection3, id, message);
			courier::handleMessage(collection4, id, message);
		}

		void add(const courier::ObjectId& id, size_t templateId)
		{
			/*We don't use the add function since we contol the add/ remove conditons*/
			(void)id;(void)templateId;
		}
		void remove(const courier::ObjectId& id) { (void)id;}
		inline size_t size() 
		{
			return collection.size() + collection2.size() + collection3.size() + collection4.size();
		}
	};

	auto* setup(auto& oc, size_t number)
	{
		for (size_t i = 0; i < number; i++)
		{
			oc.emplace_back();
		}
		auto it = oc.rbegin();
		return &(*it);
	}

	void run2(courier::Courier<Collection>* oc, std::chrono::seconds dur)
	{
		auto start = std::chrono::high_resolution_clock::now();
//		std::atomic<int> counter = 0;
		while (1)
		{
			oc->beginFrame();
			{
				oc->post(1);

				//if (counter == 10)
				{
				//	counter = 0;
					// add work onto scheduling thread
					oc->post(customType2{});

				}
			}
			oc->endFrame();
			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> elapsed = end - start;
			if (elapsed >= dur)
			{
				break;
			}
		}
	}


	void runBench(courier::Courier<Collection>* instance, size_t numSubscribers, std::chrono::seconds dur, size_t times = 4)
	{
		msgStat min{ 0,999999999999,0 };
		msgStat max{ 0,0,0 };
		msgStat avg{ 0,0,0 };
		msgStat min2{ 0,999999999999,0 };
		msgStat max2{ 0,0,0 };
		msgStat avg2{ 0,0,0 };


		/*auto ex =*/ setup(instance->getCollection().collection, numSubscribers);
		auto ex =setup(instance->getCollection().collection2, numSubscribers);
		setup(instance->getCollection().collection3, numSubscribers);
		setup(instance->getCollection().collection4, numSubscribers);

		for (size_t i = 0; i < times; i++)
		{
			std::cout << "Iteration: " << i+1 << std::endl;
			run2(instance, dur);

			if (ex->counter < min.msgcount)
			{
				min.set(dur.count(), ex->counter);
				min2.set(dur.count(), ex->counter2);
			}

			if (ex->counter > max.msgcount)
			{
				max.set(dur.count(), ex->counter);
				max2.set(dur.count(), ex->counter2);
			}

			avg.add(dur.count(), ex->counter);
			avg2.add(dur.count(), ex->counter2);

			ex->counter = 0;
			ex->counter2 = 0;
		}

		std::cout << "Test results: " << std::endl;
		std::cout << "Total Subscribers: " << instance->getCollection().size() << std::endl;
		std::cout << "Max: " << max.msgcount + max2.msgcount << "("<< max2.msgcount<<")" << ", msg/s: " << max.avgCount + max2.avgCount << "("<< max2.avgCount<<")" << std::endl;
		std::cout << "Min: " << min.msgcount + min2.msgcount << "("<< min2.msgcount<<")" << ", msg/s: " << min.avgCount + min2.avgCount << "("<< min2.avgCount<<")" << std::endl;
		std::cout << "Avg: " << avg.msgcount / times + avg2.msgcount / times << "("<< avg2.msgcount/times<<")" << ", msg/s: " << avg.avgCount + avg2.avgCount << "("<<avg2.avgCount<<")" << std::endl;
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

	void run(int cores, size_t numItems, int times)
	{

		using namespace std::chrono_literals;

		std::chrono::duration d = 2s;

		{
			courier::Courier<Collection>* instance;
			if (cores > 0)
			{
				instance = new courier::Courier<Collection>(courier::Settings{ courier::ThreadingSettings::Fixed, cores, 16 });
			}
			else
			{
				instance = new courier::Courier<Collection>(courier::Settings{ courier::ThreadingSettings::Auto, -1, 1});
			}
			g_instance = instance;
			{
				std::cout << "Testing courier "<< times <<" times, using " << getCores(cores) << " with " << numItems << " subscribers per collection " << d.count() << " s" << std::endl;

				runBench(instance, numItems, d, times);
				std::cout << std::endl;
			}

			delete instance;
		}

	}
}
namespace courier
{
	template<>
	[[using gnu: hot, flatten]] void handleObjectMessage(stresstest::exampleSubscriber2& ex, const stresstest::customType2)
	{
		(void) ex;
		ex.counter2++;
		//static int counter = 10;
		//if(counter-- == 0)
		//{
		//	counter = 10;
		//	stresstest::g_instance->scheduleFunction([&ex](){ex.counter2++;});
		//}
	}
}