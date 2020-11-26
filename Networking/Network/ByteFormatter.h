#pragma once

#include <cstddef>
#include <string>
#include <vector>

class IByteFormatter
{
    public:
        virtual ~IByteFormatter(){}

        virtual std::vector<std::byte> FormatObjectAsByte(void* obj) = 0;
};

template <typename T>
class IByteTypeFormatter : public IByteFormatter
{
    public:
        virtual std::vector<std::byte> FormatTypeAsByte(T* data) = 0;
};

class StringByteFormatter : public IByteTypeFormatter<std::string>
{
    public:
        std::vector<std::byte> FormatObjectAsByte(void* obj) override;
        std::vector<std::byte> FormatTypeAsByte(std::string* str) override;
};