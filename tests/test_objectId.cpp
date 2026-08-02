#include <courier/objectId.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Courier::ObjectId always unique", "[objectId]")
{
    courier::ObjectId o1;
    courier::ObjectId o2;
    REQUIRE(o1.id() != o2.id());
}

SCENARIO("An Object SubId always increments internal id by 1", "[objectId]")
{

    GIVEN("An ObjectId")
    {
        courier::ObjectId o1;
        WHEN("object.subId() is called")
        {
            courier::ObjectId osub = o1.newSubId();
            THEN("the new objectId.id() is offset by one")
            {
                REQUIRE(o1.id() + 1 == osub.id());
            }
            AND_THEN("object parentId is the same")
            {
                REQUIRE(o1.parentId() == osub.parentId());
            }
        }
        WHEN("object.subId() is called twice")
        {
            courier::ObjectId osub = o1.newSubId();
            courier::ObjectId osub2 = o1.newSubId();
            THEN("the new object.id() is not the same")
            {
                REQUIRE(osub.id() != osub2.id());
            }
            AND_THEN("object parentId is the same")
            {
                REQUIRE(o1.parentId() == osub.parentId());
                REQUIRE(o1.parentId() == osub2.parentId());
            }
        }
        WHEN("objectId.rawId() is called")
        {
            uint64_t rawId = o1.rawId();
            REQUIRE(rawId != 0);
            THEN("A reconstructed objectId is the same as the object rawId() was called from")
            {
                courier::ObjectId reconstructed = courier::ObjectId::from(rawId);
                REQUIRE(o1.id() == reconstructed.id());
            }
        }
    }
}
