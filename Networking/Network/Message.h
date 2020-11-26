#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "ByteFormatter.h"
#include "ByteParser.h"

class IMessage
{
    public:
        virtual ~IMessage(){}

        virtual std::vector<std::byte> AsBytes() = 0;

        virtual std::shared_ptr<void> AsObject() = 0;
};

template <typename T>
class IMessageType : public IMessage
{
    public:
        virtual std::shared_ptr<T> AsType() = 0;

        void SetByteFormatter(std::unique_ptr<IByteTypeFormatter<T>> formatter)
        {
            _byteFormatter = move(formatter);
        }

        void SetByteParser(std::unique_ptr<IByteTypeParser<T>> parser)
        {
            _byteParser = move(parser);
        }

    protected:
        std::unique_ptr<IByteTypeFormatter<T>> _byteFormatter;
        std::unique_ptr<IByteTypeParser<T>> _byteParser;
};

class StringMessage : public IMessageType<std::string>
{
    public:
        StringMessage(std::unique_ptr<IByteTypeFormatter<std::string>> formatter, std::string message);
        StringMessage(std::unique_ptr<IByteTypeParser<std::string>> parser, std::vector<std::byte> bytes);

        std::vector<std::byte> AsBytes() override;

        std::shared_ptr<void> AsObject() override;
        std::shared_ptr<std::string> AsType() override;

    private:
        std::shared_ptr<std::string> _message;
        std::vector<std::byte> _bytes;
};