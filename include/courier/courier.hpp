#pragma once

#include <map>
#include <mutex>

#include <courier/message.hpp>
#include <courier/scheduledMessage.hpp>
#include <courier/channel.hpp>
#include <courier/multiChannel.hpp>
#include <courier/subscriber.hpp>
#include <courier/topic.hpp>

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

		SubscriberId addSubscriber(const Topic topic, const Subscriber& subscriber);
		void removeSubscriber(const Topic topic, SubscriberId subscriberId);

		void createChannel(const Topic topic);
		std::shared_ptr <MultiChannel> getChannel(const Topic topic);

		Courier& operator=(const Courier) = delete;

		void scheduleRemoval(const Topic, const SubscriberId subscriberId);
		void handleScheduledMessages();
		void handleScheduledRemovals();

		size_t messageCount();
		size_t getScheduledMessageCount(const Topic topic) const;
		size_t getScheduledMessageCount() const;
	private:

		size_t m_messages = 0;
		std::mutex mtx;

		std::map<Topic, std::shared_ptr<MultiChannel>> channels;
		std::map<Topic, std::vector<internal::ScheduledMessage>> scheduledMessages;

	};

	void init();
	void shutdown();
	Courier& get();

	const std::string getVersion();
}