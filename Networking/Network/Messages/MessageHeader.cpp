#include "MessageHeader.h"

namespace Message
{
	MessageHeader::MessageHeader()
		: _messageTypeHash(0)
		, _messageSize(0)
	{}

	MessageHeader::MessageHeader(int messageTypeHash, uint32_t messageSize)
		: _messageTypeHash(messageTypeHash)
		, _messageSize(messageSize)
	{
		_bytes.reserve(HeaderClassSize());
	}

	MessageHeader::MessageHeader(const MessageHeader& other)
		: _messageTypeHash(other._messageTypeHash)
		, _messageSize(other._messageSize)
	{}

	int MessageHeader::GetTypeHash()
	{
		return _messageTypeHash;
	}

	uint32_t MessageHeader::GetSize()
	{
		return _messageSize;
	}

	void MessageHeader::FromBytes(std::vector<std::byte> bytes)
	{
		_bytes = bytes;

		// in reality, we would need a byte converter to handle endian-ness
		memcpy(&_messageTypeHash, &bytes[0], sizeof(int));
		memcpy(&_messageSize, &bytes[0], sizeof(uint32_t));
	}

	std::vector<std::byte> MessageHeader::AsBytes()
	{
		// in reality, we would need a byte converter to handle endian-ness
		if (_bytes.empty())
		{
			memcpy(&_bytes[0], &_messageTypeHash, sizeof(int));
			memcpy(&_bytes[0], &_messageSize, sizeof(uint32_t));
		}

		return _bytes;
	}
}