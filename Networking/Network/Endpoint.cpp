#include "Endpoint.h"

#include <string>

Endpoint::Endpoint()
{}

Endpoint::Endpoint(std::string address, std::string port, Protocol protocol)
: _address(address)
, _port(port)
, _protocol(protocol)
{}

std::string Endpoint::GetAddress()
{
    return _address;
}

void Endpoint::SetAddress(std::string address)
{
    _address = address;
}

uint Endpoint::GetPortAsInt()
{
    return std::atoi(_port.c_str());
}

std::string Endpoint::GetPort()
{
    return _port;
}

void Endpoint::SetPort(std::string port)
{
    _port = port;
}

Endpoint::Protocol Endpoint::GetProtocol()
{
    return _protocol;
}

void Endpoint::SetProtocol(Protocol protocol)
{
    _protocol = protocol;
}