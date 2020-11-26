#include "Socket.h"

#ifdef __WIN32__
# include "Windows/WindowsSocket.h"
#else
# include "Linux/LinuxSocket.h"
#endif

Socket::Socket()
{
#ifdef __WIN32__
    _socket = std::make_shared<WindowsSocket>();
#else
    _socket = std::make_shared<LinuxSocket>();
#endif
}

Socket::~Socket()
{
    Disconnect();
}

bool Socket::Bind(Endpoint endpoint)
{
    _bound = _socket->Bind(endpoint);

    return _bound;
}

bool Socket::Connect(Endpoint endpoint)
{
    _connected = _socket->Connect(endpoint);

    return _connected;
}

void Socket::Listen(int count)
{
    _socket->Listen(count);
}

std::unique_ptr<ISocket> Socket::Accept()
{
    return _socket->Accept();
}

int Socket::Send(std::vector<std::byte> bytes)
{
    return _socket->Send(bytes);
}

int Socket::SendTo(ISocket *socket, std::vector<std::byte> bytes)
{
    return _socket->SendTo(socket, bytes);
}

int Socket::Receive(std::vector<std::byte> &bytes)
{
    return _socket->Receive(bytes);
}

int Socket::ReceiveFrom(ISocket *socket, std::vector<std::byte> &bytes)
{
    return _socket->ReceiveFrom(socket, bytes);
}

void Socket::Disconnect()
{
    if (_connected)
    {
        _socket->Disconnect();
        _connected = false;
    }
}

void Socket::Unbind()
{
    if (_bound)
    {
        _socket->Unbind();
        _bound = false;
    }
}