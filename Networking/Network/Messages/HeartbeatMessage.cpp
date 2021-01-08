#include "HeartbeatMessage.h"

#include "Serialization/Serialization.h"

namespace Message
{
	HeartbeatMessage::HeartbeatMessage()
	{
		_type = std::make_shared<HeartbeatData>();
	}

	HeartbeatMessage::HeartbeatMessage(ConnectionData data)
	{
		_type = std::make_shared<HeartbeatData>(data);
	}

	int HeartbeatMessage::GetMessageHash()
	{
		// this should be a proper hashing function on the type's name as a string in the future
		// then this method's implementation can be macro'd to remove boilerplate code
		return 5;
	}

	uint32_t HeartbeatMessage::GetMessageSize()
	{
		return AsBytes().size();
	}

	void HeartbeatMessage::FromBytes(std::vector<std::byte> bytes)
	{
		_bytes = bytes;
		_type = std::make_shared<HeartbeatData>(Serialization::Deserialize<HeartbeatData>(bytes));
	}

	std::vector<std::byte> HeartbeatMessage::AsBytes()
	{
		// in reality, we would need a byte converter to handle endian-ness
		if (_bytes.empty())
		{
			_bytes = Serialization::Serialize(*(_type));
		}

		return _bytes;
	}

	std::shared_ptr<void> HeartbeatMessage::AsObject()
	{
		return static_cast<std::shared_ptr<void>>(AsType());
	}

	std::shared_ptr<HeartbeatData> HeartbeatMessage::AsType()
	{
		if (_type.get() == nullptr)
		{
			FromBytes(_bytes);
		}

		return _type;
	}
}