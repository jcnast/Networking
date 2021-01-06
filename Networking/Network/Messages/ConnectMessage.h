#pragma once

#include "Message.h"
#include "ConnectionData.h"

namespace Message
{
	class ConnectMessage : public IMessageType<ConnectionData>
	{
	public:
		ConnectMessage();
		ConnectMessage(const ConnectionData& data);

		int GetMessageHash() override;
		uint32_t GetMessageSize() override;

		void FromBytes(std::vector<std::byte> bytes) override;

		std::vector<std::byte> AsBytes() override;

		std::shared_ptr<void> AsObject() override;
		std::shared_ptr<ConnectionData> AsType() override;
	};
}