#include <courier/logger.hpp>

namespace courier
{
	static std::shared_ptr<courier::Logger> g_logger = nullptr;

	void setLogger(std::shared_ptr<courier::Logger> newLogger)
	{
		g_logger = newLogger;
	}

	void clearLogger()
	{
		g_logger = nullptr;
	}

	void info(const std::string_view log)
	{
		if (g_logger)
			g_logger->info(log);
	}
	void warning(const std::string_view log)
	{
		if (g_logger)
			g_logger->warning(log);
	}
	void error(const std::string_view log)
	{
		if (g_logger)
			g_logger->error(log);
	}
}