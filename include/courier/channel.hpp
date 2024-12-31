#pragma once
#include <courier/channelId.hpp>

#include <courier/message.hpp>
#include <courier/subscriber.hpp>
#include <vector>


namespace courier
{
	class Channel
	{
	public:

		Channel();
		Channel(size_t reserveSize);

		virtual ~Channel();
		virtual size_t sendMessage(const Message& message);
		virtual size_t sendMessage(const SubscriberId subscriberId, const Message& message);
		
		SubscriberId addSubscriber(const Subscriber& subscriber);
		void removeSubscriber(const SubscriberId subscriberId);

		ChannelId getId() const;

		size_t getSubscribersCount() const;
		void scheduleRemoval(const SubscriberId subscriberId);

		void handleScheduledRemovals();

		void setMultiThreaded(const bool bEnabled);

	protected:

		bool mMultithreadedEnabled;
		std::vector<Subscriber> subscribers;
		std::vector<SubscriberId> scheduledRemovals;
	private:
		ChannelId id;
	};

}