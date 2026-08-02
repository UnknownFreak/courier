#pragma once

#include "courier/objectId.hpp"
#include <vector>
#include <courier/messageHandler.hpp>


    struct message {};

    struct exampleSubscriberSmall
	{
		courier::ObjectId id;

		int counter;

		exampleSubscriberSmall() : id(), counter(0)
		{
		}
		explicit exampleSubscriberSmall(const courier::ObjectId& id) : id(id), counter(0)
		{
		}

		~exampleSubscriberSmall() = default;

		inline courier::ObjectId getId() const { return id; }

	};

    struct exampleSubscriberMedium
	{
		courier::ObjectId id;

		int counter;
        int bloat[6];

		exampleSubscriberMedium() : id(), counter(0)
		{
		}
		explicit exampleSubscriberMedium(const courier::ObjectId& id) : id(id), counter(0)
		{
		}

		~exampleSubscriberMedium() = default;

		inline courier::ObjectId getId() const { return id; }

	};

    struct exampleSubscriberLarge
	{
		courier::ObjectId id;

		int counter;
        int bloat[14];

		exampleSubscriberLarge() : id(), counter(0)
		{
		}

		explicit exampleSubscriberLarge(const courier::ObjectId& id) : id(id), counter(0)
		{
		}


		~exampleSubscriberLarge() = default;

		inline courier::ObjectId getId() const { return id; }

	};

	struct CollectionSmall
	{
		std::vector<exampleSubscriberSmall> collection;

		template<class Type>
		void onMessage(const Type& message)
		{
			courier::handleMessage(collection, message);
		}

		void add(const courier::ObjectId& id, size_t) { collection.push_back(exampleSubscriberSmall(id));};
		void remove(courier::ObjectId& id) { (void)id;}
	};

    struct CollectionMedium
	{
		std::vector<exampleSubscriberMedium> collection;

		template<class Type>
		void onMessage(const Type& message)
		{
			courier::handleMessage(collection, message);
		}

		void add(const courier::ObjectId& id, size_t) { collection.push_back(exampleSubscriberMedium(id));};
		void remove(courier::ObjectId& id) { (void)id;}
	};

    struct CollectionLarge
	{
		std::vector<exampleSubscriberLarge> collection;

		template<class Type>
		void onMessage(const Type& message)
		{
			courier::handleMessage(collection, message);
		}

		void add(const courier::ObjectId& id, size_t) { collection.push_back(exampleSubscriberLarge(id));};
		void remove(courier::ObjectId& id) { (void)id;}
	};

namespace courier
{
	template<>
	[[using gnu: hot, flatten]] void handleObjectMessage(exampleSubscriberSmall& ex, const message &)
	{
		ex.counter++;
	}
	template<>
	[[using gnu: hot, flatten]] void handleObjectMessage(exampleSubscriberMedium& ex, const message &)
	{
		ex.counter++;
	}
	template<>
	[[using gnu: hot, flatten]] void handleObjectMessage(exampleSubscriberLarge& ex, const message &)
	{
		ex.counter++;
	}

}