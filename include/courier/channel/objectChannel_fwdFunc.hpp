#pragma once

#include <courier/channel/abstractChannel.hpp>
#include <courier/subscriber.hpp>
#include <courier/util.hpp>

#include <map>
#include <vector>
#include <functional>

#ifdef _WIN32
#include <ppl.h>
#endif

#include <omp.h>

namespace courier
{
	template<class T, typename Fn> requires concepts::getId<T, SubscriberId> && std::derived_from<T, Subscriber>
	class ObjectChannelFFunc : public AbstractChannel
	{
	public:

		ObjectChannelFFunc(Topic t) : topic(t) {}

		void onMessage(const Topic messageTopic, const Message& message) override
		{
			if (messageTopic == topic)
			{
				if (isMultiThreadedEnabled)
				{
#ifdef _WIN32
					if (isOpenMpUsed)
					{
						int index;
						#pragma omp parallel for
						for (index = 0; index < objects.size(); index++)
						{
							func(objects[index], message);
						}
					}
					else
					{
						concurrency::parallel_for((size_t)0, objects.size(),
							[&](size_t index)
							{
								func(objects[index], message);
							});
					}
#else
					size_t index;
					#pragma omp parallel for
					for (index = 0; index < objects.size(); index++)
					{
						func(objects[index], message);
					}
#endif

				}
				else
				{
					for (auto& o : objects)
					{
						func(o, message);
					}
				}
			}
			else
			{
				if (topicCallbacks.find(messageTopic) != topicCallbacks.end())
				{

					auto& cb = topicCallbacks[messageTopic];

					if (isMultiThreadedEnabled)
					{
#ifdef _WIN32
						if (isOpenMpUsed)
						{
							int index;
							#pragma omp parallel for
							for (index = 0; index < objects.size(); index++)
							{
								cb(objects[index], message);
							}
						}
						else
						{
							concurrency::parallel_for((size_t)0, objects.size(),
								[&](size_t index)
								{
									cb(objects[index], message);
								});
						}
#else
						size_t index;
						#pragma omp parallel for
						for (index = 0; index < objects.size(); index++)
						{
							cb(objects[index], message);
						}
#endif

					}
					else
					{
						for (auto& o : objects)
						{
							cb(o, message);
						}
					}
				}
			}
		}

		void onMessage(const Topic messageTopic, const SubscriberId id, const Message& message) override
		{
			T* object = findObject(id);
			if (object != nullptr && topicCallbacks.find(messageTopic) != topicCallbacks.end())
			{
				auto& cb = topicCallbacks[messageTopic];
				cb(*object, message);
			}
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

		void setTopicCallback(Topic _topic, std::function<void(T&,const Message&)> _func)
		{
			topicCallbacks[_topic] = _func;
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

		std::map<Topic, std::function<void(T&, const Message&)>> topicCallbacks;
		std::vector<T> objects;
		std::vector<SubscriberId> objectsToErase;

		Topic topic;
		[[no_unique_address]] Fn func;
	};
}