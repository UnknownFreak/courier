#pragma once

#include "courier/subscriberId.hpp"
#include <vector>
#include <omp.h>
#include <courier/logger.hpp>

namespace courier
{

    template<class Object, class Message>
    void handleObjectMessage(Object&, const Message&)
    {
        #if defined COURIER_ALLOW_EMPTY_HANDLER == 0
        static_assert(false, "Template not specialized");
        #else
            #if defined COURIER_LOG_EMPTY_HANDLER == 1
            static bool b= true;
            if(b)
            {
                #ifdef _WIN32
                courier::info("TOOD ... windows template args resolving");
                #else
                std::string s = "handleObjectMessage doing nothing for: ";
                s.append(__PRETTY_FUNCTION__);
                courier::info(s);
                #endif
                b = false;
            }
            #endif
        #endif
    }

    template<class Object, class MessageType>
    void handleMessage(std::vector<Object>& v, const MessageType& message)
    {
#ifdef _WIN32
            int index;
#else
            size_t index;
#endif
        #pragma omp parallel for
        for (index = 0; index < v.size(); index++)
        {
            courier::handleObjectMessage(v[index], message);
        }
    }

    template<class Object, class MessageType>
    void handleMessage(std::vector<Object>& v, SubscriberId id, const MessageType& message)
    {
        for (size_t index = 0; index < v.size(); index++)
        {
            if(id == v[index].id)
            {
                courier::handleObjectMessage(v[index], message);
            }
        }
    }

}