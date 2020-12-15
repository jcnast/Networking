#pragma once

#ifdef __WIN32__
#include <winsock2.h>
#include <ws2tcpip.h>

#include <vector>
#include <cstddef>

#include "../ISocket.h"

class WindowsSocket : public ISocket
{
    public:
        WindowsSocket();
        WindowsSocket(const WindowsSocket &other);
		WindowsSocket(SOCKET socket, sockaddr_in socketAddr);

        WindowsSocket& operator=(const WindowsSocket &other);

		bool GetBlocking() override;
		void SetBlocking(bool blocking) override;

        bool Bind(Endpoint endpoint, bool blocking) override;
        bool Connect(Endpoint endpoint, bool blocking) override;

        void Listen(int count) override;
        std::unique_ptr<ISocket> Accept() override;

        int Send(std::vector<std::byte> bytes) override;
        int SendTo(ISocket *socket, std::vector<std::byte> bytes) override;
        int SendTo(WindowsSocket *socket, std::vector<std::byte> bytes);

        int Receive(std::vector<std::byte> &bytes) override;
        int ReceiveFrom(ISocket *socket, std::vector<std::byte> &bytes) override;
        int ReceiveFrom(WindowsSocket *socket, std::vector<std::byte> &bytes);

        void Disconnect() override;
        void Unbind() override;

    private:
        SOCKET _socket = INVALID_SOCKET;
        sockaddr_in _socketAddr;
        addrinfo *_addrinfo = NULL;
		bool _blocking = false;

        Endpoint _currentEndpoint;

        void CreateSocket(Endpoint endpoint);

        void Abort();
};

class WSAWrapper_Static
{
    public:
        static void VerifyCreated();
        static int GetLastError();

    private:
        class Constructor
        {
            public:
                Constructor();
                ~Constructor();
        };

        static Constructor _constructor;

        static WSADATA _wsaData;
        static bool _wsaActive;
        
        static void CreateWSA();
        static void CloseWSA();
};
#endif