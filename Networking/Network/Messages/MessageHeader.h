#pragma once

#include <cstring>
#include <memory>
#include <vector>

namespace Message
{
	class MessageHeader
	{
	public:
		MessageHeader(int messageTypeHash, uint32_t messageSize);
		MessageHeader(const MessageHeader& other);

		int GetTypeHash();
		uint32_t GetSize();

		void FromBytes(std::vector<std::byte> bytes);

		std::vector<std::byte> AsBytes();

		static uint32_t HeaderClassSize()
		{
			return (sizeof(int) + sizeof(uint32_t));
		}

	private:
		int _messageTypeHash;
		uint32_t _messageSize;

		std::vector<std::byte> _bytes;
	};
}