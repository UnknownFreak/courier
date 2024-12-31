#pragma once

#include <memory>
#include <string_view>

namespace courier
{

	class Logger
	{
	public:
		virtual ~Logger() = default;
		virtual void info(const std::string_view) = 0;
		virtual void warning(const std::string_view) = 0;
		virtual void error(const std::string_view) = 0;

	};


	void setLogger(std::shared_ptr<courier::Logger> newLogger);
	void clearLogger();

	void info(const std::string_view);
	void warning(const std::string_view);
	void error(const std::string_view);

}