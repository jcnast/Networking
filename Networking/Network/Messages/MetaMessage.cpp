#include "MetaMessage.h"

namespace Message
{
	MetaMessage::MetaMessage()
	{
		_type = std::make_shared<std::vector<std::shared_ptr<IMessage>>>();
	}

	int MetaMessage::GetMessageHash()
	{
		// this should be a proper hashing function on the type's name as a string in the future
		// then this method's implementation can be macro'd to remove boilerplate code
		return 6;
	}

	uint32_t MetaMessage::GetMessageSize()
	{
		return AsBytes().size();
	}

	void MetaMessage::FromBytes(std::vector<std::byte> bytes)
	{
		_bytes = bytes;
		
		// parse bytes
	}

	std::vector<std::byte> MetaMessage::AsBytes()
	{
		// in reality, we would need a byte converter to handle endian-ness
		if (_bytes.empty() || _recalculateBytes)
		{
			_bytes.reserve(TotalMessageSize());

			uint32_t byteOffset = 0;
			for (int i = 0; i < _type->size(); i++)
			{
				std::vector<std::byte> messageBytes = (*_type)[i]->AsBytes();
				int messageSize = messageBytes.size();

				copy(messageBytes.begin(), messageBytes.end(), _bytes.begin() + byteOffset);
			}
		}

		_recalculateBytes = false;
		return _bytes;
	}

	std::shared_ptr<void> MetaMessage::AsObject()
	{
		return static_cast<std::shared_ptr<void>>(AsType());
	}

	std::shared_ptr<std::vector<std::shared_ptr<WrappedMessage>>> MetaMessage::AsType()
	{
		if (_type.get() == nullptr)
		{
			FromBytes(_bytes);
		}

		return _type;
	}

	void MetaMessage::AddWrappedMessage(std::shared_ptr<WrappedMessage> message)
	{
		_recalculateBytes = true;
		_type->push_back(move(message));
	}

	uint32_t MetaMessage::TotalMessageSize()
	{
		uint32_t size;

		for (int i = 0; i < _type->size(); i++)
		{
			size += (*_type)[i]->GetMessageSize();
		}

		return size;
	}
}