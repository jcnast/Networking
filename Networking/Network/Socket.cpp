#include "Socket.h"

#ifdef __WIN32__
# include "Windows/WindowsSocket.h"
#else
# include "Linux/LinuxSocket.h"
#endif

#include "Logging/Logger.h"

Socket::Socket()
{
#ifdef __WIN32__
    _socket = std::make_shared<WindowsSocket>();
#else
    _socket = std::make_shared<LinuxSocket>();
#endif
}

Socket::Socket(std::shared_ptr<ISocket> socket)
	: _socket(socket)
	, _connected(socket.get() != nullptr)
{}

Socket::~Socket()
{
    // must be manually disconnected to avoid error when passing around copies
}

bool Socket::Active()
{
	return _connected;
}

bool Socket::GetBlocking()
{
	return _socket->GetBlocking();
}

void Socket::SetBlocking(bool blocking)
{
	_socket->SetBlocking(blocking);
}

bool Socket::Bind(Endpoint endpoint, bool blocking)
{
    _bound = _socket->Bind(endpoint, blocking);

    return _bound;
}

bool Socket::Connect(Endpoint endpoint, bool blocking)
{
    _connected = _socket->Connect(endpoint, blocking);

    return _connected;
}

void Socket::Listen(int count)
{
	if (!_bound)
	{
		Logging::LogError("Socket", "Not bound, can't listen");
		return;
	}
    _socket->Listen(count);
}

std::unique_ptr<ISocket> Socket::Accept()
{
	if (!_bound)
	{
		Logging::LogError("Socket", "Not connected, can't accept");
		return nullptr;
	}
    return _socket->Accept();
}

Socket Socket::AcceptSocket()
{
	return Socket(move(Accept()));
}

int Socket::Send(std::vector<std::byte> bytes)
{
	if (!_connected && !_bound)
	{
		Logging::LogError("Socket", "Not connected, can't send");
		return -1;
	}
    return _socket->Send(bytes);
}

int Socket::SendTo(ISocket *socket, std::vector<std::byte> bytes)
{
	if (!_connected && !_bound)
	{
		Logging::LogError("Socket", "Not connected, can't send to");
		return -1;
	}
    return _socket->SendTo(socket, bytes);
}

int Socket::Receive(std::vector<std::byte> &bytes)
{
	if (!_connected && !_bound)
	{
		Logging::LogError("Socket", "Not connected, can't receive");
		return -1;
	}
    return _socket->Receive(bytes);
}

int Socket::ReceiveFrom(ISocket *socket, std::vector<std::byte> &bytes)
{
	if (!_connected && !_bound)
	{
		Logging::LogError("Socket", "Not connected, can't receive from");
		return -1;
	}
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