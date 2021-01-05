#include "StringMessage.h"

namespace Message
{
	StringMessage::StringMessage(std::string message)
	{
		_message = std::make_unique<std::string>(message);
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
		_message = std::make_shared<std::string>(_bytes.size(), '0');
		for (size_t i = 0; i < _bytes.size(); i++)
		{
			(*_message)[i] = (char)_bytes[i];
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
			char *charStr = _message->data();
			for (int i = 0; i < _message->length(); i++)
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
		if (_message == nullptr)
		{
			_message = std::make_shared<std::string>(_bytes.size(), '0');
			for (size_t i = 0; i < _bytes.size(); i++)
			{
				(*_message)[i] = (char)_bytes[i];
			}
		}

		return std::make_shared<std::string>(*_message);
	}
}