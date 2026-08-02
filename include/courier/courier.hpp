#pragma once


#include <functional>

#include <mutex>
#include <string_view>
#include <courier/settings.hpp>
#include <courier/objectId.hpp>

#include "omp.h"

namespace courier
{
	template<class Collection>
	class Courier
	{
	public:

		explicit Courier(const Settings& _settings) : settings(_settings)
		{
			static int maxThreads = omp_get_max_threads();

			if (settings.threadSettings == courier::ThreadingSettings::Fixed)
			{
				int numThreads = maxThreads;
				if (settings.numThreads < maxThreads)
				{
					numThreads = settings.numThreads;
				}
				omp_set_num_threads(numThreads);
			}
		}
		Courier& operator=(const Courier) = delete;

		/// <summary>
		/// Post a message to all subscribers on a topic
		/// </summary>
		/// <param name="topic">The topic to send the message to</param>
		/// <param name="message">The message to send</param>
		template<class MessageType>
		[[using gnu: hot, flatten]] void post(const MessageType& message)
		{
			m_messages++;
			coll.onMessage(message);
		}

		template<class MessageType>
		void schedule(const MessageType& message)
		{
			std::lock_guard<std::mutex> lock(mtx);
			scheduledMessages.push_back([this, message](){
				coll.onMessage(message);
			});
		}

		template<class MessageType>
		void schedule(ObjectId id, const MessageType& message)
		{
			std::lock_guard<std::mutex> lock(mtx);
			scheduledMessages.push_back([this, id, message](){
				coll.onMessage(id, message);
			});
		}

		void scheduleFunction(const std::function<void(void)>& fun)
		{
			std::lock_guard<std::mutex> lock(mtx);
			scheduledMessages.push_back([this, fun](){
				fun();
			});
		}

		bool add(size_t templateId = 0)
		{
			return scheduledAdditions.push(templateId);
		}

		bool remove(uint64_t id)
		{
			return scheduledRemovals.push(id);
		}

		[[using gnu: hot, flatten]] void beginFrame()
		{
			if(scheduledAdditions.size() == 0)
			{
				return;
			}
			size_t templateId;
			while(scheduledAdditions.pop(templateId))
			{
				coll.add(ObjectId(), templateId);
			}
		}

		[[using gnu: hot, flatten]] void endFrame()
		{
			while(scheduledMessages.size() != 0)
			{
				auto local = scheduledMessages;
				scheduledMessages.clear();
				for(auto& i: local)
				{
					i();
				}
			}
			if(scheduledRemovals.size() == 0)
			{
				return;
			}
			uint64_t id;
			while(scheduledRemovals.pop(id))
			{
				coll.remove(ObjectId::from(id));
			}
		}

		size_t messageCount();
		size_t getScheduledMessageCount();

		Collection& getCollection() { return coll;}

		/*
		Statistics getStatistics() 
		{
			return {m_messages,
				scheduledMessages.size(),
				scheduledRemovals.size(),
				coll.getIdMapping(),
				coll.getNumSubscribers(),
				coll.getSubscriberHandledMessages()
			};
		};
		*/

	private:
		Collection coll;

		size_t m_messages = 0;
		const Settings settings;
		std::mutex mtx;

		vector<std::function<void(void)>> scheduledMessages;
		queue<size_t, 1024> scheduledAdditions;
		queue<uint64_t, 1024> scheduledRemovals;

	};

	const std::string_view getVersion();
}