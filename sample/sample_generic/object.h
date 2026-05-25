#pragma once


#include "courier/subscriberId.hpp"

namespace sample::generic
{

    struct deltaTime {float dt;};
    struct deleteObject {};
    struct createObject {int objectType;};

    struct object
    {
        courier::SubscriberId id;
        courier::SubscriberId getId() const { return id;}
    };

}