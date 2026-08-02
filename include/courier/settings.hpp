#pragma once

namespace courier
{

	enum class ThreadingSettings
	{
		Auto,
		Fixed,
	};

	struct Settings
	{
		ThreadingSettings threadSettings = ThreadingSettings::Auto;
		int numThreads = -1;
	};
}

#if !defined(COURIER_NO_CONFIG_HEADER) && defined(__has_include)
    #if !__has_include("courier_config.hpp")
        #define COURIER_NO_CONFIG_HEADER
    #endif
#endif

#if !defined(COURIER_NO_CONFIG_HEADER)
    #include "courier_config.hpp"
#else

#include <vector>
#include <courier/wrapper/queue_wrapper.hpp>
#include <queue>


namespace courier
{
    // to be compatible with lock free queues, use wrapper for queues that follow the standard library format.
    // The lock free queue expects the push(), and pop() functions to return bool
    template<class T, size_t N = 0>
    using queue = wrapper::queue<std::queue<T>>;

    template<class T, size_t N = 0>
    using vector = std::vector<T>;
}

#endif

#if !defined(COURIER_NO_PROFILER_HEADER) && defined(__has_include)
    #if !__has_include("courier_profiler.hpp")
        #define COURIER_NO_PROFILER_HEADER
    #endif
#endif

#if !defined(COURIER_NO_PROFILER_HEADER)
    #include "courier_profiler.hpp"
#else

namespace courier
{
    constexpr inline bool is_profiling_enabled(){return false;}
	
}

#endif