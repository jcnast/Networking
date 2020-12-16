#ifdef __WIN32__
#include "WindowsSocket.h"

#include "Logging/Logger.h"

WindowsSocket::WindowsSocket()
{
    WSAWrapper_Static::VerifyCreated();
}

WindowsSocket::WindowsSocket(const WindowsSocket &other)
: _socket(other._socket)
, _socketAddr(other._socketAddr)
, _currentEndpoint(other._currentEndpoint)
{
    WSAWrapper_Static::VerifyCreated();
}

WindowsSocket::WindowsSocket(SOCKET socket, sockaddr_in socketAddr)
: _socket(socket)
, _socketAddr(socketAddr)
{
    WSAWrapper_Static::VerifyCreated();
}

WindowsSocket& WindowsSocket::operator=(const WindowsSocket &other)
{
    _socket = other._socket;
    _socketAddr = other._socketAddr;
    _currentEndpoint = other._currentEndpoint;

    WSAWrapper_Static::VerifyCreated();

    return *this;
}

bool WindowsSocket::GetBlocking()
{
	return _blocking;
}

void WindowsSocket::SetBlocking(bool blocking)
{
	u_long blockingVal = blocking ? 0 : 1;

	if (ioctlsocket(_socket, FIONBIO, &blockingVal) != NO_ERROR)
	{
		Abort();
		return;
	}

	_blocking = blocking;
}

bool WindowsSocket::Bind(Endpoint endpoint, bool blocking)
{
    CreateSocket(endpoint);
    if (bind(_socket, _addrinfo->ai_addr, (int)_addrinfo->ai_addrlen) == SOCKET_ERROR)
    {
        Abort();
		return false;
    }

	SetBlocking(blocking);

	return true;
}

bool WindowsSocket::Connect(Endpoint endpoint, bool blocking)
{
   CreateSocket(endpoint);
    if (connect(_socket, _addrinfo->ai_addr, (int)_addrinfo->ai_addrlen) == SOCKET_ERROR)
    {
        Abort();
		return false;
    }

	SetBlocking(blocking);

	return true;
}

void WindowsSocket::Listen(int count)
{
    if (listen(_socket, count) == SOCKET_ERROR)
    {
        Abort();
    }
}

std::unique_ptr<ISocket> WindowsSocket::Accept()
{
    sockaddr_in acceptedAddr;
	int acceptedAddrLen = sizeof(acceptedAddr);
    SOCKET acceptedSocket = accept(_socket, (sockaddr*)&acceptedAddr, &acceptedAddrLen);
    if (acceptedSocket == INVALID_SOCKET)
    {
		return nullptr;
    }

    return std::make_unique<WindowsSocket>(acceptedSocket, acceptedAddr);
}

int WindowsSocket::Send(std::vector<std::byte> bytes)
{
    int sent = send(_socket, (char*)&(bytes[0]), bytes.size(), 0);

	// in production, this should be if-def'd out since it should never occur (ideally)
	if (sent == SOCKET_ERROR)
    {
		Logging::Log("WindowsSocket", "Failed with error: " + std::to_string(WSAWrapper_Static::GetLastError()));
    }

    return sent;
}

int WindowsSocket::SendTo(ISocket *socket, std::vector<std::byte> bytes)
{
    return SendTo(static_cast<WindowsSocket*>(socket), bytes);
}

int WindowsSocket::SendTo(WindowsSocket *socket, std::vector<std::byte> bytes)
{
    int sent = sendto(_socket, (char*)&(bytes[0]), bytes.size(), 0, (sockaddr*)&(socket->_socketAddr), (int)sizeof(socket->_socketAddr));

	// in production, this should be if-def'd out since it should never occur (ideally)
    if (sent == SOCKET_ERROR)
    {
		Logging::Log("WindowsSocket", "Failed with error: " + std::to_string(WSAWrapper_Static::GetLastError()));
    }

    return sent;
}

int WindowsSocket::Receive(std::vector<std::byte> &bytes)
{
	int received = recv(_socket, (char*)&(bytes[0]), bytes.size(), 0);

	// in production, this should be if-def'd out since it should never occur (ideally)
	if (received == SOCKET_ERROR && WSAWrapper_Static::GetLastError() != WSAEWOULDBLOCK)
	{
		Logging::Log("WindowsSocket", "Failed with error: " + std::to_string(WSAWrapper_Static::GetLastError()));
	}

	return received;
}

int WindowsSocket::ReceiveFrom(ISocket *socket, std::vector<std::byte> &bytes)
{
    return ReceiveFrom(static_cast<WindowsSocket*>(socket), bytes);
}

int WindowsSocket::ReceiveFrom(WindowsSocket *socket, std::vector<std::byte> &bytes)
{
	int socketAddrLen = sizeof(socket->_socketAddr);
    int received = recvfrom(_socket, (char*)&(bytes[0]), bytes.size(), 0, (sockaddr*)&(socket->_socketAddr), &socketAddrLen);

	// in production, this should be if-def'd out since it should never occur (ideally)
    if (received == SOCKET_ERROR && WSAWrapper_Static::GetLastError() != WSAEWOULDBLOCK)
    {
		Logging::Log("WindowsSocket", "Failed with error: " + std::to_string(WSAWrapper_Static::GetLastError()));
    }

    return received;
}

void WindowsSocket::Disconnect()
{
    freeaddrinfo(_addrinfo);
    if (shutdown(_socket, SD_BOTH) == SOCKET_ERROR)
    {
        Abort();
    }
}

void WindowsSocket::Unbind()
{
    Disconnect();
}

void WindowsSocket::CreateSocket(Endpoint endpoint)
{
    addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));

    switch (endpoint.GetProtocol())
    {
        case Endpoint::Protocol::TCP:
        {
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;
			break;
        }
        case Endpoint::Protocol::UDP:
        {
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_DGRAM;
            hints.ai_protocol = IPPROTO_UDP;
			break;
        }
        default:
        {
            Abort();
			return;
        }
    }

    freeaddrinfo(_addrinfo);

    if (getaddrinfo(endpoint.GetAddress().c_str(), endpoint.GetPort().c_str(), &hints, &_addrinfo) != 0)
    {
        Abort();
		return;
    }

    _socket = socket(_addrinfo->ai_family, _addrinfo->ai_socktype, _addrinfo->ai_protocol);
    
    _socketAddr.sin_family = AF_INET;
    _socketAddr.sin_addr.s_addr = inet_addr(endpoint.GetAddress().c_str());
    _socketAddr.sin_port = htons(endpoint.GetPortAsInt());

    _currentEndpoint = endpoint;

    if (_socket == INVALID_SOCKET)
    {
        Abort();
		return;
    }
}

void WindowsSocket::Abort()
{
	Logging::Log("WindowsSocket", "Failed with error: " + std::to_string(WSAWrapper_Static::GetLastError()));
    if (_addrinfo != NULL)
    {
        freeaddrinfo(_addrinfo);
        _addrinfo = NULL;
    }
    if (_socket != INVALID_SOCKET)
    {
        closesocket(_socket);
        _socket = INVALID_SOCKET;
    }
}

WSADATA WSAWrapper_Static::_wsaData;
bool WSAWrapper_Static::_wsaActive = false;
WSAWrapper_Static::Constructor WSAWrapper_Static::_constructor = Constructor();

WSAWrapper_Static::Constructor::Constructor()
{
    CreateWSA();
}

WSAWrapper_Static::Constructor::~Constructor()
{
    CloseWSA();
}

void WSAWrapper_Static::VerifyCreated()
{
    if (!_wsaActive)
    {
		Logging::Log("WindowsSocket", "WSA Not Active!");
    }
}

void WSAWrapper_Static::CreateWSA()
{
    if (!_wsaActive)
    {
        _wsaActive = WSAStartup(MAKEWORD(2,2), &_wsaData) == 0;
    }

	if (!_wsaActive)
	{
		Logging::Log("WindowsSocket", "Failed to create WSA with error: " + std::to_string(WSAWrapper_Static::GetLastError()));
	}
	else
	{
		Logging::Log("WindowsSocket", "Created WSA");
	}
}

void WSAWrapper_Static::CloseWSA()
{
    if (_wsaActive)
    {
        WSACleanup();
    }
}

int WSAWrapper_Static::GetLastError()
{
    return WSAGetLastError();
}
#endif