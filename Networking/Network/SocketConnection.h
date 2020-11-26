#pragma once

#include <memory>

#include "Socket.h"
#include "Message.h"

class IReceiver
{
    public:
        virtual std::unique_ptr<IMessage> GetMessage() = 0;
};

class ISender
{
    public:
        virtual void SendMessage(std::unique_ptr<IMessage> message) = 0;
};

class IConnection
{
    public:
        virtual void Connect(Socket socket, Endpoint endpoint) = 0;
        virtual void Connect(Endpoint endpoint) = 0;

        virtual std::vector<std::unique_ptr<IMessage>> Disconnect(bool flush = false) = 0;
};