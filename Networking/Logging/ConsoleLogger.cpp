#include "ConsoleLogger.h"

#include <iostream>
#include <stdexcept>

namespace Logging
{
	void ConsoleLogger::Log(std::string tag, std::string message)
	{
		std::cout << "I: [" << tag << "] " << message << std::endl;
	}


	void ConsoleLogger::LogWarning(std::string tag, std::string message)
	{
		std::cout << "W: [" << tag << "] " << message << std::endl;
	}

	void ConsoleLogger::LogError(std::string tag, std::string message)
	{
		std::cout << "E: [" << tag << "] " << message << std::endl;
	}

	void ConsoleLogger::ThrowException(std::string tag, std::string message)
	{
		throw std::runtime_error("[" + tag + "] : " + message);
	}
}