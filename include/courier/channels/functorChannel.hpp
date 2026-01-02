#pragma once

#include <courier/channel.hpp>
#include <courier/subscriberId.hpp>
#include <courier/util.hpp>

#include <map>
#include <vector>
#include <functional>

#include <omp.h>

#include <courier/logger.hpp>

namespace courier
{
	class FunctorChannel : public Channel
	{
	public:
		void onMessage(const Topic messageTopic, const Message& message) override
		{
			if (topicCallbacks.find(messageTopic) != topicCallbacks.end())
			{
				auto& cb = topicCallbacks[messageTopic];

#ifdef _WIN32
				int index;
#else
				size_t index;
#endif
				#pragma omp parallel for
				for (index = 0; index < cb.size(); index++)
				{
					cb[index](message);
				}
			}
		}

		void onMessage(const Topic messageTopic, const SubscriberId id, const Message& message) override
		{
			(void)messageTopic;
			(void)id;
			(void)message;
			info("onMessage(topic, id, message) not supported");
		}

		virtual void handleScheduledRemovals() override
		{

		}

		void addTopicCallback(Topic topic, std::function<void(const Message&)> func)
		{
			topicCallbacks[topic].emplace_back(func);
		}

	private:

		std::map<Topic, std::vector<std::function<void(const Message&)>>> topicCallbacks;

	};
}