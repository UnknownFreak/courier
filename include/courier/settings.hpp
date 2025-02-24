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
		ThreadingSettings threadSettings;
		int numThreads = -1;
	};
}