#include "ConsoleLogger.h"

#include <iostream>
#include <stdexcept>

namespace Logging
{
	void ConsoleLogger::Log(std::string tag, std::string message)
	{
		_coutMutex.lock();
		{
			std::cout << "I: [" << tag << "] " << message << std::endl;
		}
		_coutMutex.unlock();
	}

	void ConsoleLogger::LogWarning(std::string tag, std::string message)
	{
		_coutMutex.lock();
		{
			std::cout << "W: [" << tag << "] " << message << std::endl;
		}
		_coutMutex.unlock();
	}

	void ConsoleLogger::LogError(std::string tag, std::string message)
	{
		_coutMutex.lock();
		{
			std::cout << "E: [" << tag << "] " << message << std::endl;
		}
		_coutMutex.unlock();
	}

	void ConsoleLogger::ThrowException(std::string tag, std::string message)
	{
		throw std::runtime_error("[" + tag + "] : " + message);
	}
}