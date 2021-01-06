#include "StringMessage.h"

namespace Message
{
	StringMessage::StringMessage()
	{}

	StringMessage::StringMessage(std::string message)
	{
		_type = std::make_shared<std::string>(message);
	}

	StringMessage::StringMessage(std::vector<std::byte> bytes)
	{
		_bytes = bytes;
	}

	int StringMessage::GetMessageHash()
	{
		// this should be a proper hashing function on the type's name as a string in the future
		// then this method's implementation can be macro'd to remove boilerplate code
		return 1;
	}

	void StringMessage::FromBytes(std::vector<std::byte> bytes)
	{
		_bytes = bytes;
		_type = std::make_shared<std::string>(_bytes.size(), '0');
		for (size_t i = 0; i < _bytes.size(); i++)
		{
			(*_type)[i] = (char)_bytes[i];
		}
	}

	uint32_t StringMessage::GetMessageSize()
	{
		return AsBytes().size();
	}

	std::vector<std::byte> StringMessage::AsBytes()
	{
		if (_bytes.empty())
		{
			char *charStr = _type->data();
			for (int i = 0; i < _type->length(); i++)
			{
				_bytes.push_back(std::byte(charStr[i]));
			}
		}

		return _bytes;
	}

	std::shared_ptr<void> StringMessage::AsObject()
	{
		return static_cast<std::shared_ptr<void>>(AsType());
	}

	std::shared_ptr<std::string> StringMessage::AsType()
	{
		if (_type.get() == nullptr || _type->empty())
		{
			FromBytes(_bytes);
		}

		return _type;
	}
}