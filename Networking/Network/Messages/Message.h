#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace Message
{
	class IMessage
	{
	public:
		virtual ~IMessage() {}

		virtual int GetMessageHash() = 0;
		virtual uint32_t GetMessageSize() = 0;

		virtual void FromBytes(std::vector<std::byte> bytes) = 0;

		virtual std::vector<std::byte> AsBytes() = 0;

		// this may just need to be a pointer to the object owned by the message
		// otherwise, self-referential message types don't work
		// unless those message types separate their data into a container (probably better actually)
		virtual std::shared_ptr<void> AsObject() = 0;
	};

	template <typename T>
	class IMessageType : public IMessage
	{
	public:
		virtual std::shared_ptr<T> AsType() = 0;
	};
}