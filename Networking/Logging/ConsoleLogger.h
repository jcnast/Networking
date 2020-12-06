#include "ILogger.h"

namespace Logging
{
	class ConsoleLogger : public ILogger
	{
	public:
		void Log(std::string tag, std::string message) override;
		void LogWarning(std::string tag, std::string message) override;
		void LogError(std::string tag, std::string message) override;
		void ThrowException(std::string tag, std::string message) override;
	};
}