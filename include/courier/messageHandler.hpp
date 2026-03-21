#pragma once

#include <vector>
#include <omp.h>


namespace courier
{

    template<class Object, class Message>
    void handleObjectMessage(Object&, const Message&);

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

}