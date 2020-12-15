#pragma once

#include <memory>
#include <vector>
#include <cstddef>

#include "Endpoint.h"

class ISocket
{
    public:
        virtual ~ISocket(){}

		virtual bool GetBlocking() = 0;
		virtual void SetBlocking(bool blocking) = 0;

        virtual bool Bind(Endpoint endpoint, bool blocking) = 0;
        virtual bool Connect(Endpoint endpoint, bool blocking) = 0;

        virtual void Listen(int count) = 0;
        virtual std::unique_ptr<ISocket> Accept() = 0;

        virtual int Send(std::vector<std::byte> bytes) = 0;
        virtual int SendTo(ISocket *socket, std::vector<std::byte> bytes) = 0;

        virtual int Receive(std::vector<std::byte> &bytes) = 0;
        virtual int ReceiveFrom(ISocket *socket, std::vector<std::byte> &bytes) = 0;

        virtual void Disconnect() = 0;
        virtual void Unbind() = 0;
};