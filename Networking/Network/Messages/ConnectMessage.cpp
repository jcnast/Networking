#include "ConnectMessage.h"

#include "Serialization/Serialization.h"

namespace Message
{
	ConnectMessage::ConnectMessage()
		: _type(std::make_shared<ConnectionData>())
	{}

	ConnectMessage::ConnectMessage(const ConnectionData& data)
		: _type(std::make_shared<ConnectionData>(data))
	{}

	int ConnectMessage::GetMessageHash()
	{
		// this should be a proper hashing function on the type's name as a string in the future
		// then this method's implementation can be macro'd to remove boilerplate code
		return 3;
	}

	uint32_t ConnectMessage::GetMessageSize()
	{
		return AsBytes().size();
	}

	void ConnectMessage::FromBytes(std::vector<std::byte> bytes)
	{
		_bytes = bytes;
		_type = std::make_shared<ConnectionData>(Serialization::Deserialize<ConnectionData>(bytes));
	}

	std::vector<std::byte> ConnectMessage::AsBytes()
	{
		// in reality, we would need a byte converter to handle endian-ness
		if (_bytes.empty())
		{
			_bytes = Serialization::Serialize(*(_type));
		}

		return _bytes;
	}

	std::shared_ptr<void> ConnectMessage::AsObject()
	{
		return static_cast<std::shared_ptr<void>>(AsType());
	}

	std::shared_ptr<ConnectionData> ConnectMessage::AsType()
	{
		if (_type.get() == nullptr)
		{
			FromBytes(_bytes);
		}

		return _type;
	}
}