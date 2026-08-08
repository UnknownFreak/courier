#pragma once

#include <courier/objectId.hpp>

#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>

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

    template<class QueueType>
    class queue2
    {
        public:
            queue2() = default;
            ~queue2() = default;

            [[using gnu: hot, flatten]] bool inline push(auto&& val)
            {
                {
                    std::unique_lock<std::mutex> lock(m_mutex);
                    m_queue.push(val);
                }
                m_cond.notify_one();

                return true;
            }

            [[using gnu: hot, flatten]] bool inline pop(auto&& val)
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                using namespace std::chrono_literals;
                if(!m_cond.wait_for(lock, 1ns, [&]{return m_queue.empty() == false;}))
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
        std::mutex m_mutex;
        std::condition_variable m_cond;
    };
}

namespace courier
{
    // to be compatible with lock free queues, use wrapper for queues that follow the standard library format.
    // The lock free queue expects the push(), and pop() functions to return bool
    template<class T, size_t N = 0>
    using queue = stresstest::queue2<std::queue<T>>;

    template<class T, size_t N = 0>
    using vector = std::vector<T>;
}