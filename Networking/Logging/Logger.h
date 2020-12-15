#include "ILogger.h"

#include <vector>
#include <memory>
#include <mutex>

namespace Logging
{
	void LogType(Logging::LogTypeEnum type, std::string tag, std::string message);

	void Log(std::string tag, std::string message);
	void LogWarning(std::string tag, std::string message);
	void LogError(std::string tag, std::string message);
	void ThrowException(std::string tag, std::string message);

	bool AddImplementation(std::shared_ptr<ILogger> implementation);
	void RemoveImplementation(std::shared_ptr<ILogger> implementation);

	class Logger : public ILogger
	{
	public:
		static Logger* Instance();;

		void Log(std::string tag, std::string message) override;
		void LogWarning(std::string tag, std::string message) override;
		void LogError(std::string tag, std::string message) override;
		void ThrowException(std::string tag, std::string message) override;

		bool AddImplementation(std::shared_ptr<ILogger> implementation);
		void RemoveImplementation(std::shared_ptr<ILogger> implementation);

	private:
		static std::unique_ptr<Logger> _instance;

		std::mutex _implementationsMutex;
		std::vector<std::shared_ptr<ILogger>> _implementations;
	};
}