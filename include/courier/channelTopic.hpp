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

		bool removeChannel(const size_t channelId);

		size_t sendMessage(const Message& message) override;

		size_t sendMessage(const size_t subscriberId, const Message& message) override;

		size_t sendMessageCh(const size_t channelId, const Message& message);

		size_t sendMessage(const size_t channelId, const size_t subscriberId, const Message& message);

		void setMessageValidator(std::shared_ptr<MessageValidator> messageValidator);
		std::shared_ptr<MessageValidator> getValidator() const;

		bool validate(const Message& message) const;

	private:
		std::map<size_t, std::shared_ptr<Channel>> channels;
		std::shared_ptr<MessageValidator> validator;
	};
}