#include <iostream>
#include <string>

#include "Network/Endpoint.h"
#include "Network/Socket.h"
#include "Network/ClientConnection.h"

#include "Network/Messages/StringMessage.h"

#include "Client.h"

#include "Logging/Logger.h"
#include "Logging/ConsoleLogger.h"

void ExecuteClientConnection()
{
	Logging::AddImplementation(std::make_shared<Logging::ConsoleLogger>());

	Logging::Log("Client", "Executing client connection");

    // may not work
    Endpoint endpoint = Endpoint("127.0.0.1", "3333", Endpoint::Protocol::TCP);
    Socket socket;

	Logging::Log("Client", "Socket and endpoint created");

    ClientConnection client;
	// client needs to be non-blocking when checking for any info from server
    client.Connect(socket, endpoint, false);

	Logging::Log("Client", "Client is connected");

    client.SendMessage(std::make_unique<Message::StringMessage>("Test Message"));

	Logging::Log("Client", "Message added to be sent");

    while(true)
    {
        std::unique_ptr<Message::IMessage> serverMessage = move(client.GetMessage());

        if (serverMessage.get() != nullptr)
        {
            std::shared_ptr<std::string> message = dynamic_cast<Message::StringMessage*>(serverMessage.get())->AsType();

			Logging::Log("Client", "Message from server: " + *message);
        }
    }
}