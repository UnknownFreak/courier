#pragma once

#include <map>

#include <courier/channel.hpp>
#include <courier/messageValidator.hpp>


namespace courier
{
	class ChannelTopic : public Channel
	{
	public:

		void addChannel(std::shared_ptr<Channel> channel);

		bool removeChannel(const ChannelId channelId);

		size_t sendMessage(const Message& message) override;

		size_t sendMessage(const SubscriberId subscriberId, const Message& message) override;

		size_t sendMessage(const ChannelId channelId, const Message& message);

		size_t sendMessage(const ChannelId channelId, const SubscriberId subscriberId, const Message& message);

		void setMessageValidator(std::shared_ptr<MessageValidator> messageValidator);
		std::shared_ptr<MessageValidator> getValidator() const;

		bool validate(const Message& message) const;

	private:
		std::map<ChannelId, std::shared_ptr<Channel>> channels;
		std::shared_ptr<MessageValidator> validator;
	};
}