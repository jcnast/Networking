#include "Logger.h"

namespace Logging
{
	// free functions
	void LogType(Logging::LogTypeEnum type, std::string tag, std::string message)
	{
		switch (type)
		{
			case LogTypeEnum::Info:
			{
				Log(tag, message);
				break;
			}
			case LogTypeEnum::Warning:
			{
				LogWarning(tag, message);
				break;
			}
			case LogTypeEnum::Error:
			{
				LogError(tag, message);
				break;
			}
			case LogTypeEnum::Exception:
			{
				ThrowException(tag, message);
				break;
			}
			default:
			{
				ThrowException("Logger", "Unsupported LogType!");
				break;
			}
		}
	}

	void Log(std::string tag, std::string message) { Logger::Instance()->Log(tag, message); }
	void LogWarning(std::string tag, std::string message) { Logger::Instance()->LogWarning(tag, message); }
	void LogError(std::string tag, std::string message) { Logger::Instance()->LogError(tag, message); }
	void ThrowException(std::string tag, std::string message) { Logger::Instance()->ThrowException(tag, message); }

	bool AddImplementation(std::shared_ptr<ILogger> implementation) { return Logger::Instance()->AddImplementation(implementation); }
	void RemoveImplementation(std::shared_ptr<ILogger> implementation) { Logger::Instance()->RemoveImplementation(implementation); }

	// member functinos
	std::unique_ptr<Logger> Logger::_instance = nullptr;

	Logger* Logger::Instance()
	{
		if (_instance.get() == nullptr)
		{
			_instance = std::make_unique<Logger>();
		}

		return _instance.get();
	}

	void Logger::Log(std::string tag, std::string message)
	{
		_implementationsMutex.lock();
		{
			for (int i = 0; i < _implementations.size(); i++)
			{
				_implementations[i]->Log(tag, message);
			}
		}
		_implementationsMutex.unlock();
	}


	void Logger::LogWarning(std::string tag, std::string message)
	{
		_implementationsMutex.lock();
		{
			for (int i = 0; i < _implementations.size(); i++)
			{
				_implementations[i]->ThrowException(tag, message);
			}
		}
		_implementationsMutex.unlock();
	}

	void Logger::LogError(std::string tag, std::string message)
	{
		_implementationsMutex.lock();
		{
			for (int i = 0; i < _implementations.size(); i++)
			{
				_implementations[i]->LogError(tag, message);
			}
		}
		_implementationsMutex.unlock();
	}

	void Logger::ThrowException(std::string tag, std::string message)
	{
		_implementationsMutex.lock();
		{
			for (int i = 0; i < _implementations.size(); i++)
			{
				_implementations[i]->ThrowException(tag, message);
			}
		}
		_implementationsMutex.unlock();
	}

	bool Logger::AddImplementation(std::shared_ptr<ILogger> implementation)
	{
		bool add = true;
		_implementationsMutex.lock();
		{
			for (int i = 0; i < _implementations.size(); i++)
			{
				if (_implementations[i] == implementation)
				{
					add = false;
				}
			}

			if (add)
			{
				_implementations.push_back(implementation);
			}
		}
		_implementationsMutex.unlock();

		return add;
	}

	void Logger::RemoveImplementation(std::shared_ptr<ILogger> implementation)
	{
		_implementationsMutex.lock();
		{
			for (int i = 0; i < _implementations.size(); i++)
			{
				if (_implementations[i] == implementation)
				{
					_implementations.erase(_implementations.begin() + i);
					return;
				}
			}
		}
		_implementationsMutex.unlock();
	}
}