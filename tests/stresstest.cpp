
#include "stresstest.hpp"

#include <courier/courier.hpp>
#include <courier/settings.hpp>
#include <courier/messageHandler.hpp>

#include <iostream>
#include <chrono>
#include <omp.h>


namespace stresstest
{

	//syntax sugaring with naming

	size_t g_id = 0;
	size_t g_id2 = 0;

	struct customType {};

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
	void handleMessage<stresstest::exampleSubscriber, float>(std::vector<stresstest::exampleSubscriber>&, const float&)
	{
		static bool once = true;
		if(once)
		{
			std::cout << "Float messages ignored for exampleSubscriber" << std::endl;
			once = false;
		}
		return;
	}

	template<>
	[[using gnu: hot, flatten]] void handleObjectMessage(stresstest::exampleSubscriber& ex, const int &)
	{
		ex.counter++;
	}

	template<>
	[[using gnu: hot, flatten]] void handleObjectMessage(stresstest::exampleSubscriber& ex, const stresstest::customType &)
	{
		ex.counter2++;
	}
	
	template<>
	[[using gnu: hot, flatten]] void handleObjectMessage(stresstest::exampleSubscriber2& ex, const int &)
	{
		ex.counter++;
	}

	template<>
	[[using gnu: hot, flatten]] void handleObjectMessage(stresstest::exampleSubscriber2& ex, const stresstest::customType &)
	{
		ex.counter2++;
	}
		
	template<>
	void handleObjectMessage(stresstest::exampleSubscriber2& , const float &)
	{
	}
}

namespace stresstest
{
	struct Collection
	{
		std::vector<exampleSubscriber> collection;
		std::vector<exampleSubscriber2> collection2;

		template<class Type>
		void onMessage(const Type& message)
		{
			courier::handleMessage(collection, message);
			courier::handleMessage(collection2, message);
		}

		void add(const courier::ObjectId& id, size_t templateId)
		{
			/*We don't use the add function since we contol the add/ remove conditons*/
			(void)id;(void)templateId;
		}
		void remove(const courier::ObjectId& id) { (void)id;}
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
		int counter = 0;
		while (1)
		{
			oc->beginFrame();
			oc->post(1);
			counter++;
			if (counter == 10)
			{
				counter = 0;
				oc->post(1.0f);
				oc->post(customType{});

			}
			oc->endFrame();
			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> elapsed = end - start;
			if (elapsed >= dur)
				break;
		}
	}


	void runBench(courier::Courier<Collection>* instance, size_t numSubscribers, std::chrono::seconds dur, size_t times = 4)
	{
		msgStat min{ 0,999999999999,0 };
		msgStat max{ 0,0,0 };
		msgStat avg{ 0,0,0 };


		auto ex = setup(instance->getCollection().collection, numSubscribers);
		setup(instance->getCollection().collection2, numSubscribers);

		for (size_t i = 0; i < times; i++)
		{
			run2(instance, dur);

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
		std::cout << "Total Subscribers: " << instance->getCollection().collection.size() + instance->getCollection().collection2.size() << std::endl;
		std::cout << "Max: " << max.msgcount << ", msg/s: " << max.avgCount << std::endl;
		std::cout << "Min: " << min.msgcount << ", msg/s: " << min.avgCount << std::endl;
		std::cout << "Avg: " << avg.msgcount / times << ", msg/s: " << avg.avgCount << std::endl;
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
				instance = new courier::Courier<Collection>(courier::Settings{ courier::ThreadingSettings::Fixed, cores });
			}
			else
			{
				instance = new courier::Courier<Collection>(courier::Settings{ courier::ThreadingSettings::Auto });
			}
			{
				std::cout << "Testing courier "<< times <<" times, using " << getCores(cores) << " with " << numItems << " subscribers for " << d.count() << " seconds" << std::endl;

				runBench(instance, numItems, d, times);
				std::cout << std::endl;
			}

			delete instance;
		}

	}
}