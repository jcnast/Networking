#pragma once

#include <string>

namespace Logging
{
	enum LogType
	{
		Info,
		Warning,
		Error,
		Exception
	};

	class ILogger
	{
	public:
		virtual void Log(std::string tag, std::string message) = 0;
		virtual void LogWarning(std::string tag, std::string message) = 0;
		virtual void LogError(std::string tag, std::string message) = 0;
		virtual void ThrowException(std::string tag, std::string message) = 0;
	};
}