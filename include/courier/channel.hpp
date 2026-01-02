#pragma once

#include <courier/topic.hpp>
#include <courier/channelId.hpp>
#include <courier/msg/message.hpp>
#include <courier/msg/messageValidator.hpp>
#include <courier/subscriberId.hpp>

#include <string>
#include <string_view>

namespace courier
{
	class Channel
	{

	public:
		/// Abstract methods definitions

		/// <summary>
		/// Abstract method to handle messages on a specific topic
		/// </summary>
		/// <param name="messageTopic">The topic the message was sent to</param>
		/// <param name="message">The message data</param>
		virtual void onMessage(const Topic messageTopic, const Message& message) = 0;
		virtual void onMessage(const Topic messageTopic, const SubscriberId id, const Message& message) = 0;

		virtual void handleScheduledRemovals() = 0;

	public:

		/// Other public methods definition

		Channel(std::string_view channelName);
		virtual ~Channel() = default;

		// prevent channels being copy assigned
		Channel& operator=(const Channel&) = delete;

		/// <summary>
		/// Gets the channel identifier associated with this channel.
		/// </summary>
		/// <returns>An unique identifier for this channel</returns>
		ChannelId getId() const;

		/// <summary>
		/// Sets a channel name, no channel name is set by default
		/// </summary>
		/// <param name="channelName"> The name of the channel</param>
		void setChannelName(std::string_view channelName);

		/// <summary>
		/// Gets the currently set channel name
		/// </summary>
		/// <returns>A string view representing the channel name</returns>
		std::string_view getChannelName() const;


		/// <summary>
		/// Sets a user defined message validator to validate messages before invoking the onMessage method
		/// </summary>
		/// <param name="messageValidator">The message validator to set</param>
		void setMessageValidator(std::shared_ptr<MessageValidator> messageValidator);


		/// <summary>
		/// Gets the currently set message validator
		/// </summary>
		/// <returns>A shared ptr to the messageValidator</returns>
		std::shared_ptr<MessageValidator> getValidator() const;


		/// <summary>
		/// Validates the message before invoking onMessage
		/// Always returns true if no messageValidator is set
		/// </summary>
		/// <param name="message">The message to validate</param>
		/// <returns>True if the message was valid, false otherwise</returns>
		bool validate(const Topic topic, const Message& message) const;

	private:

		std::shared_ptr<MessageValidator> validator;

		ChannelId channelId;
	protected:
		std::string channelName;
	};
}