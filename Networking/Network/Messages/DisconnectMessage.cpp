#include "DisconnectMessage.h"

namespace Message
{
	DisconnectMessage::DisconnectMessage()
		: _type(std::make_shared<ConnectionData>())
	{}

	DisconnectMessage::DisconnectMessage(ConnectionData data)
		: _type(std::make_shared<ConnectionData>(data))
	{}

	int DisconnectMessage::GetMessageHash()
	{
		// this should be a proper hashing function on the type's name as a string in the future
		// then this method's implementation can be macro'd to remove boilerplate code
		return 4;
	}

	uint32_t DisconnectMessage::GetMessageSize()
	{
		return AsBytes().size();
	}

	void DisconnectMessage::FromBytes(std::vector<std::byte> bytes)
	{
		_bytes = bytes;
		_type = std::make_shared<ConnectionData>(Serialization::Deserialize<ConnectionData>(bytes));
	}

	std::vector<std::byte> DisconnectMessage::AsBytes()
	{
		// in reality, we would need a byte converter to handle endian-ness
		if (_bytes.empty())
		{
			_bytes = Serialization::Serialize(*(_type));
		}

		return _bytes;
	}

	std::shared_ptr<void> DisconnectMessage::AsObject()
	{
		return static_cast<std::shared_ptr<void>>(AsType());
	}

	std::shared_ptr<ConnectionData> DisconnectMessage::AsType()
	{
		if (_type.get() == nullptr)
		{
			FromBytes(_bytes);
		}

		return _type;
	}
}