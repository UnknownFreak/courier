#pragma once

#include <courier/objectId.hpp>

#include <vector>
#include <queue>

namespace stresstest
{
   template<class QueueType>
    class queue
    {
        public:
            queue() = default;
            ~queue() = default;

            [[using gnu: hot, flatten]] bool inline push(auto&& val)
            {
                m_queue.push(val);
                return true;
            }

            [[using gnu: hot, flatten]] bool inline pop(auto&& val)
            {
                if(m_queue.size() == 0)
                {
                    return false;
                }
                val = m_queue.front();
                m_queue.pop();
                return true;
            }

            [[using gnu: hot, flatten]] size_t size() const
            {
                return m_queue.size();
            }

        private:
        QueueType m_queue;
    };
}

namespace courier
{
    // to be compatible with lock free queues, use wrapper for queues that follow the standard library format.
    // The lock free queue expects the push(), and pop() functions to return bool
    template<class T, size_t N = 0>
    using queue = stresstest::queue<std::queue<T>>;

    template<class T, size_t N = 0>
    using vector = std::vector<T>;
}