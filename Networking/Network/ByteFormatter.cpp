#include "ByteFormatter.h"

std::vector<std::byte> StringByteFormatter::FormatObjectAsByte(void* obj)
{
    return FormatTypeAsByte(static_cast<std::string*>(obj));
}

std::vector<std::byte> StringByteFormatter::FormatTypeAsByte(std::string* str)
{
    std::vector<std::byte> strBytes;
    
    char *charStr = str->data();
    for (int i = 0; i < str->length(); i++)
    {
        strBytes.push_back(std::byte(charStr[i]));
    }

    return strBytes;
}