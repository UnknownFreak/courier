#pragma once

#include <courier/settings.hpp>

#include <functional>

#include <mutex>
#include <vector>
#include <memory>

#include "omp.h"

namespace courier
{
	template<class Collection>
	class Courier
	{
	public:

		explicit Courier(const Settings& _settings) : settings(_settings)
	//		Courier::Courier(const Settings& in_settings) : settings(in_settings)
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

/*		/// <summary>
		/// Post a message to a specific subscriber on a topic
		/// </summary>
		/// <param name="topic">The topic to send the message to</param>
		/// <param name="subscriber">The specific subscriberId to send the message to, 0 sends to all</param>
		/// <param name="message">The message to send</param>
		template<class Message>
		void post(const SubscriberId subscriber, const Message& message)
		{
			for(auto& channel: channels)
			{
				channel->onMessage(subscriber, message);
			}
		}

		/// <summary>
		/// Post a message to a specific subscriber on a topic
		/// </summary>
		/// <param name="topic">The topic to send the message to</param>
		/// <param name="channel">The specific channel to send the message to, 0 sends to all</param>
		/// <param name="message">The message to send</param>
		template<class Message>
		void post(const ChannelId channelId, const Message& message)
		{
			for(auto& channel: channels)
			{
				if(channel->getId() == channelId)
					channel->onMessage(message);
			}
		}

		/// <summary>
		/// Post a message to a specific subscriber on a specific sub channel on a topic
		/// </summary>
		/// <param name="topic">The topic to send the message to</param>
		/// <param name="channel">The specific channel to send to, 0 sends to all channels</param>
		/// <param name="subscriberId">The specific channel to send to, 0 sends to all subscribers to selected channel</param>
		/// <param name="message">The message to send</param>
		template<class Message>
		void post(const ChannelId channelId, const SubscriberId subscriberId, const Message& message)
		{
			for(auto& channel: channels)
			{
				if(channel->getId() == channelId)
					channel->onMessage(subscriberId, message);
			}
		}


		/// <summary>
		/// Schedule a message to all subscribers on a topic
		/// </summary>
		/// <param name="topic">The topic to schedule the message to</param>
		/// <param name="message">The message to schedule</param>
		template<class Message>
		void schedule(const Message& message)
		{
			scheduledMessages.push_back([this, &message](){
				post(message);
			});
		}

		/// <summary>
		/// Schedule a message to a specific subscriber on a topic
		/// </summary>
		/// <param name="topic">The topic to schedule the message to</param>
		/// <param name="subscriber">The specific subscriberId to schedule the message to, 0 schedules to all</param>
		/// <param name="message">The message to schedule</param>
		template<class Message>
		void schedule(const SubscriberId subscriberId, const Message& message)
		{
			scheduledMessages.push_back([this, subscriberId, &message](){
				post(subscriberId, message);
			});
		}
		
		/// <summary>
		/// Schedule a message to a specific subscriber on a topic
		/// </summary>
		/// <param name="topic">The topic to schedule the message to</param>
		/// <param name="channel">The specific channel to schedule the message to, 0 schedules to all</param>
		/// <param name="message">The message to schedule</param>
		template<class Message>
		void schedule(const ChannelId channel, const Message& message)
		{
			scheduledMessages.push_back([this, channel, &message](){
				post(channel, message);
			});
		}
		
		/// <summary>
		/// Schedule a message to a specific subscriber on a specific sub channel on a topic
		/// </summary>
		/// <param name="topic">The topic to schedule the message to</param>
		/// <param name="channel">The specific channel to schedule to, 0 schedules to all channels</param>
		/// <param name="subscriberId">The specific channel to send to, 0 schedules to all</param>
		/// <param name="message">The message to schedule</param>
		template<class Message>
		void schedule(const ChannelId channel, const SubscriberId subscriberId, const Message& message)
		{
			scheduledMessages.push_back([this, channel,subscriberId, &message](){
				post(channel, subscriberId, message);
			});
		}
		*/

		void handleScheduledMessages();
		void handleScheduledRemovals();

		size_t messageCount();
		size_t getScheduledMessageCount();

		Collection& getCollection() { return coll;}
	private:
		Collection coll;

		size_t m_messages = 0;
		const Settings settings;
		std::mutex mtx;

		std::vector<std::function<void(void)>> scheduledMessages;

	};

	void init(const Settings& in_settings);
	void shutdown();
	//Courier& get();

	const std::string getVersion();
}