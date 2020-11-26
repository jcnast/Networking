#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

class IByteParser
{
    public:
        virtual ~IByteParser(){}
    
        virtual std::shared_ptr<void> ParseBytesToObject(std::vector<std::byte> bytes) = 0;
};

template <typename T>
class IByteTypeParser : public IByteParser
{
    public:
        virtual std::shared_ptr<T> ParseBytesToType(std::vector<std::byte> bytes) = 0;
};

class StringByteParser : public IByteTypeParser<std::string>
{
    public:
        std::shared_ptr<void> ParseBytesToObject(std::vector<std::byte> bytes) override;
        std::shared_ptr<std::string> ParseBytesToType(std::vector<std::byte> bytes) override;
};