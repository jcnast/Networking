#pragma once

#include "Message.h"
#include "MessageHeader.h"

namespace Message
{
	class WrappedMessage : public IMessageType<IMessage>
	{
	public:
		WrappedMessage();
		WrappedMessage(MessageHeader header, std::unique_ptr<IMessage> message);

		int GetMessageHash() override;
		uint32_t GetMessageSize() override;

		void FromBytes(std::vector<std::byte> bytes) override;

		std::vector<std::byte> AsBytes() override;

		std::shared_ptr<void> AsObject() override;
		std::shared_ptr<IMessage> AsType() override;

	private:
		MessageHeader _header;
	};

	template <typename T, typename ...Ts>
	WrappedMessage WrapMessage(Ts... args)
	{
		std::unique_ptr<T> messageToBeWrapped = std::unique_ptr<T>(args...);
		return WrappedMessage(CreateMessageHeader(messageToBeWrapped.get()), move(messageToBeWrapped));
	}

	// static so that it can only be called from this header file
	// pointer as we know it is only called from the above method, lifecycle does not need to be managed here
	static MessageHeader CreateMessageHeader(IMessage* message)
	{
		// it may be nice to have the GetMessageHash portion not rely on having an instance, instead we could pass in a type and operate based on that
		// but, that may be unnecessary as it may overcomplicate implemenations and not result in much, if any, gain
		return MessageHeader(message->GetMessageHash(), message->GetMessageSize());
	}
}