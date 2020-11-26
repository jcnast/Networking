#pragma once

#include <string>

using uint = unsigned int;

class Endpoint
{
    public:
        enum class Protocol
        {
            TCP,
            UDP
        };

        Endpoint();
        Endpoint(std::string address, std::string port, Protocol protocol);

        std::string GetAddress();
        void SetAddress(std::string address);

        uint GetPortAsInt();
        std::string GetPort();
        void SetPort(std::string port);

        Protocol GetProtocol();
        void SetProtocol(Protocol protocol);

    private:
        std::string _address;
        std::string _port;
        Protocol _protocol;
};