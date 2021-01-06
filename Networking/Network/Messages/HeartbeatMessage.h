#pragma once

#include "Message.h"
#include "ConnectionData.h"

#include <chrono>

namespace Message
{
	class HeartbeatData : public ConnectionData
	{
	public:
		HeartbeatData()
			: ConnectionData()
			, _messageSentTime(GetCurrentTime())
		{}

		HeartbeatData(uint32_t clientID, uint32_t serverID)
			: ConnectionData(clientID, serverID)
			, _messageSentTime(GetCurrentTime())
		{}

		HeartbeatData(const ConnectionData& connection)
			: ConnectionData(connection)
			, _messageSentTime(GetCurrentTime())
		{}

		HeartbeatData(const HeartbeatData& other)
			: ConnectionData((ConnectionData)other)
			, _messageSentTime(other._messageSentTime)
		{}

		uint64_t MessageSentTime()
		{
			return _messageSentTime;
		}

	private:
		uint64_t _messageSentTime;

		static uint64_t GetCurrentTime()
		{
			std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
			std::chrono::system_clock::duration timeSinceEpoch = currentTime.time_since_epoch();

			return (uint64_t)timeSinceEpoch.count();
		}
	};

	class HeartbeatMessage : public IMessageType<HeartbeatData>
	{
	public:
		HeartbeatMessage();
		HeartbeatMessage(ConnectionData data);

		int GetMessageHash() override;
		uint32_t GetMessageSize() override;

		void FromBytes(std::vector<std::byte> bytes) override;

		std::vector<std::byte> AsBytes() override;

		std::shared_ptr<void> AsObject() override;
		std::shared_ptr<HeartbeatData> AsType() override;
	};
}