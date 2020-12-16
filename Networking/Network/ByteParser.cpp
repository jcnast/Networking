#include "ByteParser.h"

std::shared_ptr<void> StringByteParser::ParseBytesToObject(std::vector<std::byte> bytes)
{
    return static_cast<std::shared_ptr<void>>(ParseBytesToType(bytes));
}

std::shared_ptr<std::string> StringByteParser::ParseBytesToType(std::vector<std::byte> bytes)
{
    std::shared_ptr<std::string> str = std::make_shared<std::string>(bytes.size(), '0');
    for (size_t i = 0; i < bytes.size(); i++)
    {
		(*str)[i] = (char)bytes[i];
    }

    return str;
}