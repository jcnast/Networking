#include "WrappedMessage.h"

#include "MessageFactory.h"

namespace Message
{
	WrappedMessage::WrappedMessage()
	{}

	WrappedMessage::WrappedMessage(MessageHeader header, std::unique_ptr<IMessage> message)
		: _header(header)
	{
		_type = move(message);
	}

	int WrappedMessage::GetMessageHash()
	{
		// this should be a proper hashing function on the type's name as a string in the future
		// then this method's implementation can be macro'd to remove boilerplate code
		return 2;
	}

	uint32_t WrappedMessage::GetMessageSize()
	{
		return MessageHeader::HeaderClassSize() + _type->GetMessageSize();
	}

	void WrappedMessage::FromBytes(std::vector<std::byte> bytes)
	{
		_bytes = bytes;

		// header from first HEADER_SIZE bytes
		_header.FromBytes(std::vector<std::byte>(bytes.begin(), bytes.begin() + MessageHeader::HeaderClassSize()));
		// get type and size from header, then create message
		std::vector<std::byte> messageBytes(bytes.begin() + MessageHeader::HeaderClassSize(), bytes.end());
		_type = Factory::ConstructMessage(_header, messageBytes);
	}

	std::vector<std::byte> WrappedMessage::AsBytes()
	{
		// in reality, we would need a byte converter to handle endian-ness
		if (_bytes.empty())
		{
			_bytes.reserve(MessageHeader::HeaderClassSize() + _header.GetSize());

			std::vector<std::byte> headerBytes = _header.AsBytes();
			copy(_bytes.begin(), _bytes.begin() + MessageHeader::HeaderClassSize(), headerBytes.begin());

			std::vector<std::byte> messageBytes = _type->AsBytes();
			copy(_bytes.begin() + MessageHeader::HeaderClassSize(), _bytes.end(), messageBytes.begin());
		}

		return _bytes;
	}

	std::shared_ptr<void> WrappedMessage::AsObject()
	{
		return static_cast<std::shared_ptr<void>>(AsType());
	}

	std::shared_ptr<IMessage> WrappedMessage::AsType()
	{
		if (_type.get() == nullptr)
		{
			FromBytes(_bytes);
		}

		return _type;
	}
}