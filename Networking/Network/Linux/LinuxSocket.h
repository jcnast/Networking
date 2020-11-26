#pragma once

#ifndef __WIN32__
# include <sys/socket.h>

#include <vector>
#include <ctsddef>

#include "../ISocket.h"

class LinuxSocket : public ISocket
{
    public:
        LinuxSocket();

        bool Bind(Endpoint endpoint) override;
        bool Connect(Endpoint endpoint) override;

        void Listen(int count) override;
        std::unique_ptr<ISocket> Accept() override;

        int Send(std::vector<std::byte> bytes) override;
        int SendTo(ISocket *socket, std::vector<std::byte> bytes) override;

        int Receive(std::vector<std::byte> &bytes) override;
        int ReceiveFrom(ISocket *socket, std::vector<std::byte> &bytes) override;

        void Disconnect() override;
        void Unbind() override;

    private:

};
#endif