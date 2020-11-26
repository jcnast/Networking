#pragma once

#include <cstddef>
#include <vector>
#include <queue>

using uint = unsigned int;

class IBuffer
{
    public:
        virtual ~IBuffer(){}

        virtual void ClearBuffer() = 0;

        virtual std::vector<std::byte> PopFromBuffer(uint numBytes) = 0;
        virtual void PushToBuffer(std::vector<std::byte> bytes) = 0;

    private:

};

class BasicBuffer : public IBuffer
{
    public:
        void ClearBuffer() override;

        std::vector<std::byte> PopFromBuffer(uint numBytes) override;
        void PushToBuffer(std::vector<std::byte> bytes) override;

    private:
        std::queue<std::byte> _buffer;
};