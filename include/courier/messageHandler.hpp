#pragma once

#include <courier/settings.hpp>
#include <courier/objectId.hpp>
#include <cstdint>
#include <vector>
#include <omp.h>
#include <courier/logger.hpp>

namespace courier
{

    template<class Message>
    void handleEvent(const Message&)
    {
        #if defined COURIER_ALLOW_EMPTY_HANDLER == 0
        static_assert(false, "Template not specialized");
        #else
            #if defined COURIER_LOG_EMPTY_HANDLER == 1
            static bool once = true;
            if(once)
            {
                #ifdef _WIN32
                courier::info("TOOD ... windows template args resolving");
                #else
                std::string s = "handleObjectMessage doing nothing for: ";
                s.append(__PRETTY_FUNCTION__);
                courier::info(s);
                #endif
                once = false;
            }
            #endif
        #endif
    }

    template<class Object, class Message>
        requires(sizeof(Message) >= sizeof(uintptr_t))
    void handleObjectMessage(Object&, const Message&)
    {
        #if defined COURIER_ALLOW_EMPTY_HANDLER == 0
        static_assert(false, "Template not specialized");
        #else
            #if defined COURIER_LOG_EMPTY_HANDLER == 1
            static bool once = true;
            if(once)
            {
                #ifdef _WIN32
                courier::info("TOOD ... windows template args resolving");
                #else
                std::string s = "handleObjectMessage doing nothing for: ";
                s.append(__PRETTY_FUNCTION__);
                courier::info(s);
                #endif
                once = false;
            }
            #endif
        #endif
    }
    template<class Object, class Message>
        requires(sizeof(Message) < sizeof(uintptr_t))
    void handleObjectMessage(Object&, const Message)
    {
    }

    template<class Object, class MessageType>
        requires(sizeof(MessageType) >= sizeof(uintptr_t))
    [[using gnu: hot, flatten]] void handleMessage(std::vector<Object>& v, const MessageType& message)
    {
#ifdef _WIN32
            int index;
#else
            size_t index;
#endif
        #pragma omp parallel for// num_threads(16)
        for (index = 0; index < v.size(); index++)
        {
            courier::handleObjectMessage(v[index], message);
        }
    }
        template<class Object, class MessageType>
        requires(sizeof(MessageType) < sizeof(uintptr_t))
    [[using gnu: hot, flatten]] void handleMessage(std::vector<Object>& v, const MessageType message)
    {
#ifdef _WIN32
            int index;
#else
            size_t index;
#endif
        #pragma omp parallel for// num_threads(16)
        for (index = 0; index < v.size(); index++)
        {
            courier::handleObjectMessage(v[index], message);
        }
    }

    template<class Object, class MessageType>
    void handleMessage(std::vector<Object>& v, ObjectId id, const MessageType& message)
    {
        for (size_t index = 0; index < v.size(); index++)
        {
            if(id.id() == v[index].id.id())
            {
                courier::handleObjectMessage(v[index], message);
                if constexpr(is_profiling_enabled())
                {
                    
                }
            }
        }
    }
}