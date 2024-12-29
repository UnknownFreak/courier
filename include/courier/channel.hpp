#pragma once

#include <vector>

#include <courier/message.hpp>
#include <courier/subscriber.hpp>


namespace courier
{
	class Channel
	{
	public:

		Channel();
		Channel(size_t reserveSize);

		virtual ~Channel();
		virtual size_t sendMessage(const Message& message);
		virtual size_t sendMessage(const size_t subscriberId, const Message& message);
		
		size_t addSubscriber(const Subscriber& subscriber);
		void removeSubscriber(const size_t subscriberId);

		size_t getId() const;

		size_t getSubscribersCount() const;
		void scheduleRemoval(const size_t subscriberId);

		void handleScheduledRemovals();

		void setMultiThreaded(const bool bEnabled);

	protected:

		bool mMultithreadedEnabled;
		std::vector<Subscriber> subscribers;
		std::vector<size_t> scheduledRemovals;
	private:
		size_t id;
	};

}