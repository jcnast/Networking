#include "Logger.h"

namespace Logging
{
	Logger* Logger::Instance()
	{
		if (_instance.get() == nullptr)
		{
			_instance = std::make_unique<Logger>();
		}

		return _instance.get();
	}

	void Logger::LogType(LogType type, std::string tag, std::string message)
	{
		switch (type)
		{
			case LogType::Info:
			{
				Log(tag, message);
				break;
			}
			case LogType::Warning:
			{
				LogWarning(tag, message);
				break;
			}
			case LogType::Error:
			{
				LogError(tag, message);
				break;
			}
			case LogType::Exception:
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
				_implementations[i]->ThrLogWarningowException(tag, message);
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

	void Logger::ThrowException(std::string tag, std::string message) override;
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
				_implementations.push_back(implementations);
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
					_implementations.erase(i);
					return;
				}
			}
		}
		_implementationsMutex.unlock();
	}
}