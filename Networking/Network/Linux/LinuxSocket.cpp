#ifndef __WIN32__
#include "LinuxSocket.h"

LinuxSocket::LinuxSocket()
{}

bool LinuxSocket::Bind(Endpoint endpoint)
{

}

bool LinuxSocket::Connect(Endpoint endpoint)
{

}

void LinuxSocket::Listen(int count)
{

}

std::unique_ptr<ISocket> LinuxSocket::Accept()
{

}

int LinuxSocket::Send(std::vector<byte> bytes)
{

}

int LinuxSocket::SendTo(ISocket *socket, std::vector<std::byte> bytes)
{

}

int LinuxSocket::Receive(std::vector<byte> &bytes)
{

}

int LinuxSocket::ReceiveFrom(ISocket *socket, std::vector<std::byte> &bytes)
{

}

void LinuxSocket::Disconnect()
{

}

void LinuxSocket::Unbind()
{
    
}
#endif