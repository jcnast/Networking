#include "Message.h"

StringMessage::StringMessage(std::unique_ptr<IByteTypeFormatter<std::string>> formatter, std::string message)
{
    SetByteFormatter(move(formatter));
    _message = std::make_unique<std::string>(message);
}

StringMessage::StringMessage(std::unique_ptr<IByteTypeParser<std::string>> parser, std::vector<std::byte> bytes)
{
    SetByteParser(move(parser));
    _bytes = bytes;
}

std::vector<std::byte> StringMessage::AsBytes()
{
    if (_bytes.empty())
    {
        _bytes = _byteFormatter->FormatTypeAsByte(_message.get());
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
        _message = _byteParser->ParseBytesToType(_bytes);
    }

    return std::make_shared<std::string>(*_message);
}