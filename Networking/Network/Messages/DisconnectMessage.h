#pragma once

#include "Message.h"
#include "ConnectionData.h"

namespace Message
{
	class DisconnectMessage : public IMessageType<ConnectionData>
	{
	public:
		DisconnectMessage();
		DisconnectMessage(ConnectionData data);

		int GetMessageHash() override;
		uint32_t GetMessageSize() override;

		void FromBytes(std::vector<std::byte> bytes) override;

		std::vector<std::byte> AsBytes() override;

		std::shared_ptr<void> AsObject() override;
		std::shared_ptr<ConnectionData> AsType() override;
	};
}