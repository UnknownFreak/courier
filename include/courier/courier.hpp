#pragma once

#include <courier/settings.hpp>

#include <functional>

#include <mutex>
#include <vector>
#include <string>

#include "courier/subscriberId.hpp"
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
		void post(const MessageType& message)
		{
			coll.onMessage(message);
		}

		template<class MessageType>
		void schedule(SubscriberId id, const MessageType& message)
		{
			std::lock_guard<std::mutex> lock(mtx);
			scheduledMessages.push_back([this, id, &message](){
				coll.onMessage(id, message);
			});
		}

		void remove(SubscriberId id)
		{
			std::lock_guard<std::mutex> lock(removeMtx);
			scheduledRemovals.emplace_back(id);
		}

		void endFrame()
		{
			for(auto& i: scheduledMessages)
			{
				i();
			}
			scheduledMessages.clear();
			coll.remove(scheduledRemovals);
			scheduledRemovals.clear();
		}

		size_t messageCount();
		size_t getScheduledMessageCount();

		Collection& getCollection() { return coll;}
	private:
		Collection coll;

		size_t m_messages = 0;
		const Settings settings;
		std::mutex mtx;
		std::mutex removeMtx;

		std::vector<std::function<void(void)>> scheduledMessages;
		std::vector<SubscriberId> scheduledRemovals;

	};

	void init(const Settings& in_settings);
	void shutdown();
	//Courier& get();

	const std::string getVersion();
}