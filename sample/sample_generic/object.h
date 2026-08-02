#pragma once


#include "courier/objectId.hpp"

namespace sample::generic
{

    struct deltaTime {float dt;};
    struct deleteObject {};

    struct object
    {
        courier::ObjectId id;
        courier::ObjectId getId() const { return id;}
    };

}