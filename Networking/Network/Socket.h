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
		Socket(std::shared_ptr<ISocket> socket);
        ~Socket();

		bool Active();

		bool GetBlocking() override;
		void SetBlocking(bool blocking) override;

        bool Bind(Endpoint endpoint, bool blocking) override;
        bool Connect(Endpoint endpoint, bool blocking) override;

        void Listen(int count) override;
        std::unique_ptr<ISocket> Accept() override;
		Socket AcceptSocket();

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