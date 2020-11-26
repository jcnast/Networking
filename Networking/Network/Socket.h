#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "Endpoint.h"
#include "ISocket.h"

class Socket : public ISocket
{
    public:
        Socket();
        ~Socket();

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
        bool _bound = false;
        bool _connected = false;

        std::shared_ptr<ISocket> _socket = nullptr;
};