#include "Buffer.h"

void BasicBuffer::ClearBuffer()
{
    _buffer.empty();
}

std::vector<std::byte> BasicBuffer::PopFromBuffer(uint numBytes)
{
    std::vector<std::byte> frontBytes;
    frontBytes.reserve(numBytes);

    for (uint i = 0; i < numBytes; i++)
    {
        frontBytes[i] = _buffer.front();
        _buffer.pop();
    }

    return frontBytes;
}

void BasicBuffer::PushToBuffer(std::vector<std::byte> bytes)
{
    for (uint i = 0; i < bytes.size(); i++)
    {
        _buffer.push(bytes[i]);
    }
}