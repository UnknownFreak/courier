#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace courier
{
    struct Statistics
    {
        size_t numPosted = 0;
        size_t numScheduledMessages = 0;
        size_t numScheduledRemovals = 0;
        std::vector<std::pair<size_t, std::string>> idMapping;
        std::vector<std::pair<size_t, size_t>> numSubscribers;
        std::vector<std::pair<size_t, size_t>> numHandledSubscriberMessages;
    };
}