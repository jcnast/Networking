#include <iostream>
#include <string>

#include "Network/Endpoint.h"
#include "Network/Socket.h"
#include "Network/ClientConnection.h"

#include "Network/Message.h"

#include "Client.h"

void ExecuteClientConnection()
{
    // may not work
    Endpoint endpoint = Endpoint("127.0.0.1", "3333", Endpoint::Protocol::TCP);
    Socket socket;

    ClientConnection client;
    client.Connect(socket, endpoint);

    std::unique_ptr<StringByteFormatter> stringFormatter = std::make_unique<StringByteFormatter>();
    client.SendMessage(std::make_unique<StringMessage>(move(stringFormatter), "Test Message"));

    while(true)
    {
        std::unique_ptr<IMessage> serverMessage = move(client.GetMessage());

        if (serverMessage.get() != nullptr)
        {
            std::shared_ptr<std::string> message = dynamic_cast<StringMessage*>(serverMessage.get())->AsType();

            std::cout << "Message from server: " << message->c_str() << std::endl;
        }
    }
}