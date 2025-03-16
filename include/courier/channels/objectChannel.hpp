#pragma once

#include <courier/channel.hpp>
#include <courier/subscriberId.hpp>
#include <courier/util.hpp>

#include <map>
#include <vector>
#include <functional>

#include <omp.h>

namespace courier
{
	template<class T, typename Fn> requires concepts::Subscriber<T, SubscriberId>
	class ObjectChannel : public Channel
	{
	public:

		void onMessage(const Topic messageTopic, const Message& message) override
		{
#ifdef _WIN32
			int index;
#else
			size_t index;
#endif
			#pragma omp parallel for
			for (index = 0; index < objects.size(); index++)
			{
				func(messageTopic, objects[index], message);
			}
		}

		void onMessage(const Topic messageTopic, const SubscriberId id, const Message& message) override
		{
			T* object = findObject(id);
			func(messageTopic, *object, message);
		}


		void handleScheduledRemovals() override
		{
			util::vectorFastRemove<SubscriberId>(objects, objectsToErase);
		}

		void reserve(size_t newSize)
		{
			objects.reserve(newSize);
		}

		void erase(SubscriberId id)
		{
			objectsToErase.emplace_back(id);
		}

		void erase(std::vector<T>::const_iterator it)
		{
			objectsToErase.emplace_back(it.getId());
		}

		template<class ...Args>
		decltype(auto) emplace_back(Args&& ... args)
		{
			return objects.emplace_back(std::forward<Args>(args)...);
		}

		void push_back(const T& val)
		{
			objects.push_back(val);
		}

		void push_back(T&& val)
		{
			objects.push_back(std::move(val));
		}

		void eraseRange(std::vector<SubscriberId> idsToErase)
		{
			std::copy(idsToErase.begin(), idsToErase.end(), std::back_inserter(objectsToErase));
		}

	private:

		T* findObject(SubscriberId id)
		{
			for (auto& o : objects)
			{
				if (o.getId() == id)
				{
					return &o;
				}
			}
			return nullptr;
		}

		std::vector<T> objects;
		std::vector<SubscriberId> objectsToErase;

		[[no_unique_address]] Fn func;
	public:

		ObjectChannel() = default;
	};
	
}