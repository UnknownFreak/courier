#pragma once

#include <condition_variable>
#include <mutex>

namespace courier::wrapper
{
    template<class QueueType>
    class queue
    {
        public:
            queue() = default;
            ~queue() = default;

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
                if(!m_cond.wait_for(lock, 0ns, [&]{return m_queue.empty() == false;}))
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