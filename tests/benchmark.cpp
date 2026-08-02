#include <courier/courier.hpp>
#include <catch2/benchmark/catch_chronometer.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include "collection.hpp"
#include "courier/settings.hpp"

TEST_CASE("Courier")
{
    BENCHMARK_ADVANCED("1 Subscriber 8Byte 1 Core")(Catch::Benchmark::Chronometer meter)
    {
        courier::Courier<CollectionSmall> c({.threadSettings=courier::ThreadingSettings::Fixed, .numThreads=1});
        c.add();
        c.beginFrame();

        meter.measure([&c]() { c.post(message{});});
    };

    BENCHMARK_ADVANCED("1 Subscriber 32Byte 1 Core")(Catch::Benchmark::Chronometer meter)
    {
        courier::Courier<CollectionMedium> c({.threadSettings=courier::ThreadingSettings::Fixed, .numThreads=1});
        c.add();
        c.beginFrame();

        meter.measure([&c]() { c.post(message{});});
    };

    BENCHMARK_ADVANCED("1 Subscriber 64Byte 1 Core")(Catch::Benchmark::Chronometer meter)
    {
        courier::Courier<CollectionLarge> c({.threadSettings=courier::ThreadingSettings::Fixed, .numThreads=1});
        c.add();
        c.beginFrame();

        meter.measure([&c]() { c.post(message{});});
    };

    BENCHMARK_ADVANCED("512 Subscribers 8Byte All Cores")(Catch::Benchmark::Chronometer meter)
    {
        courier::Courier<CollectionSmall> c({});
        for(size_t i =0; i < 512; i++)
        {
            c.add();
        }
        c.beginFrame();

        meter.measure([&c]() { c.post(message{});});
    };

    BENCHMARK_ADVANCED("512 Subscribers 32Byte All Cores")(Catch::Benchmark::Chronometer meter)
    {
        courier::Courier<CollectionMedium> c({});
        for(size_t i =0; i < 512; i++)
        {
            c.add();
        }
        c.beginFrame();

        meter.measure([&c]() { c.post(message{});});
    };

    BENCHMARK_ADVANCED("512 Subscribers 64Byte All Cores")(Catch::Benchmark::Chronometer meter)
    {
        courier::Courier<CollectionLarge> c({});
        for(size_t i =0; i < 512; i++)
        {
            c.add();
        }
        c.beginFrame();

        meter.measure([&c]() { c.post(message{});});
    };
}

