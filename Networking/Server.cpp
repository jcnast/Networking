#include <iostream>
#include <string>

#include "Network/Endpoint.h"
#include "Network/Socket.h"
#include "Network/ServerConnection.h"

#include "Network/Message.h"

#include "Server.h"

void ExecuteServerConnection()
{
    // may not work
    Endpoint endpoint = Endpoint("127.0.0.1", "3333", Endpoint::Protocol::TCP);
    Socket socket;

    ServerConnection server;
    server.Connect(socket, endpoint);

    while(true)
    {
        std::unique_ptr<IMessage> clientMessage = move(server.GetMessage());

        if (clientMessage != nullptr)
        {
            std::shared_ptr<std::string> message = dynamic_cast<StringMessage*>(clientMessage.get())->AsType();

            std::cout << "Message from client: " << message->c_str() << std::endl;
        }
    }
}