#include <iostream>
#include <string>

#include "Network/Endpoint.h"
#include "Network/Socket.h"
#include "Network/ServerConnection.h"

#include "Network/Message.h"

#include "Server.h"

#include "Logging/Logger.h"
#include "Logging/ConsoleLogger.h"

void ExecuteServerConnection()
{
	Logging::AddImplementation(std::make_shared<Logging::ConsoleLogger>());

	Logging::Log("Server", "Executing server connection");

    // may not work
    Endpoint endpoint = Endpoint("127.0.0.1", "3333", Endpoint::Protocol::TCP);
    Socket socket;

	Logging::Log("Server", "Socket and endpoint created");

    ServerConnection server;
	server.SetMaxClients(10);
	// server shouldn't _need_ to be blocking, if it does - may be that we need to use the select() function
    server.Connect(socket, endpoint, false);

	Logging::Log("Server", "Server is connected");

    while(true)
    {
        std::unique_ptr<IMessage> clientMessage = move(server.GetMessage());

        if (clientMessage != nullptr)
        {
            std::shared_ptr<std::string> message = dynamic_cast<StringMessage*>(clientMessage.get())->AsType();

			Logging::Log("Server", "Message from client: " + *message);
        }
    }
}