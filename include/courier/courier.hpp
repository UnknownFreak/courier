#pragma once

#include <courier/channel/abstractChannel.hpp>

#include <courier/msg/message.hpp>
#include <courier/msg/scheduledMessage.hpp>

#include <courier/topic.hpp>

#include <map>
#include <mutex>
#include <vector>

namespace courier
{
	class Courier
	{
	public:

		/// <summary>
		/// Post a message to all subscribers on a topic
		/// </summary>
		/// <param name="topic">The topic to send the message to</param>
		/// <param name="message">The message to send</param>
		void post(const Topic topic, const Message& message);

		/// <summary>
		/// Post a message to a specific subscriber on a topic
		/// </summary>
		/// <param name="topic">The topic to send the message to</param>
		/// <param name="subscriber">The specific subscriberId to send the message to, 0 sends to all</param>
		/// <param name="message">The message to send</param>
		void post(const Topic topic, const SubscriberId subscriber, const Message& message);

		/// <summary>
		/// Post a message to a specific subscriber on a topic
		/// </summary>
		/// <param name="topic">The topic to send the message to</param>
		/// <param name="channel">The specific channel to send the message to, 0 sends to all</param>
		/// <param name="message">The message to send</param>
		void post(const Topic topic, const ChannelId channel, const Message& message);

		/// <summary>
		/// Post a message to a specific subscriber on a specific sub channel on a topic
		/// </summary>
		/// <param name="topic">The topic to send the message to</param>
		/// <param name="channel">The specific channel to send to, 0 sends to all channels</param>
		/// <param name="subscriberId">The specific channel to send to, 0 sends to all subscribers to selected channel</param>
		/// <param name="message">The message to send</param>
		void post(const Topic topic, const ChannelId channel, const SubscriberId subscriberId, const Message& message);

		/// <summary>
		/// Schedule a message to all subscribers on a topic
		/// </summary>
		/// <param name="topic">The topic to schedule the message to</param>
		/// <param name="message">The message to schedule</param>
		void schedule(const Topic topic, const Message& message);

		/// <summary>
		/// Schedule a message to a specific subscriber on a topic
		/// </summary>
		/// <param name="topic">The topic to schedule the message to</param>
		/// <param name="subscriber">The specific subscriberId to schedule the message to, 0 schedules to all</param>
		/// <param name="message">The message to schedule</param>
		void schedule(const Topic topic, const SubscriberId subscriberId, const Message& message);
		
		/// <summary>
		/// Schedule a message to a specific subscriber on a topic
		/// </summary>
		/// <param name="topic">The topic to schedule the message to</param>
		/// <param name="channel">The specific channel to schedule the message to, 0 schedules to all</param>
		/// <param name="message">The message to schedule</param>
		void schedule(const Topic topic, const ChannelId channel, const Message& message);
		
		/// <summary>
		/// Schedule a message to a specific subscriber on a specific sub channel on a topic
		/// </summary>
		/// <param name="topic">The topic to schedule the message to</param>
		/// <param name="channel">The specific channel to schedule to, 0 schedules to all channels</param>
		/// <param name="subscriberId">The specific channel to send to, 0 schedules to all</param>
		/// <param name="message">The message to schedule</param>
		void schedule(const Topic topic, const ChannelId channel, const SubscriberId subscriberId, const Message& message);

		void addChannel(std::shared_ptr<AbstractChannel> channel);
		bool removeChannel(const ChannelId channelId);

		Courier& operator=(const Courier) = delete;

		void handleScheduledMessages();
		void handleScheduledRemovals();

		size_t messageCount();
		size_t getScheduledMessageCount(const Topic topic);
		size_t getScheduledMessageCount();
	private:

		size_t m_messages = 0;
		std::mutex mtx;

		std::vector<std::shared_ptr<AbstractChannel>> channels;
		std::map<Topic, std::vector<internal::ScheduledMessage>> scheduledMessages;

	};

	void init();
	void shutdown();
	Courier& get();

	const std::string getVersion();
}