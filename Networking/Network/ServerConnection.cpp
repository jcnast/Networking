#include "ServerConnection.h"

#include "Logging/Logger.h"

ServerConnection::~ServerConnection()
{
	Disconnect();
}

std::unique_ptr<IMessage> ServerConnection::GetMessage()
{
    std::unique_ptr<IMessage> oldestMessage = nullptr;

    _receivedMutex.lock();
    {
        oldestMessage = (_receivedMessages.size() == 0) ? nullptr : move(_receivedMessages.front());

        if (oldestMessage != nullptr)
        {
            _receivedMessages.pop();
        }
    }
    _receivedMutex.unlock();

    return oldestMessage;
}

void ServerConnection::SendMessage(std::unique_ptr<IMessage> message)
{
    _sendMutex.lock();
    {
        _messagesToSend.push(move(message));
    }
    _sendMutex.unlock();
}

void ServerConnection::SetMaxClients(int maxClients)
{
	_maxClients = maxClients;
	_clientHandlers = std::make_unique<ClientHandler[]>(_maxClients);
}

void ServerConnection::Connect(Socket socket, Endpoint endpoint, bool blocking)
{
    if (_threadRunning)
    {
        Disconnect();
    }

    _socket = socket;
    Connect(endpoint, blocking);
}

void ServerConnection::Connect(Endpoint endpoint, bool blocking)
{
    if (_threadRunning)
    {
        Disconnect();
    }

    _socket.Bind(endpoint, blocking);
	_socket.Listen(10);

	_threadShouldStop = false;
    _thread = std::thread(&ServerConnection::Run, this);
	_threadRunning = true;
}

std::vector<std::unique_ptr<IMessage>> ServerConnection::Disconnect(bool flush)
{
    while (flush)
    {
        _sendMutex.lock();
        {
            flush = _messagesToSend.empty();
        }
        _sendMutex.unlock();
    }

	_handlersMutex.lock();
	{
		for (int i = 0; i < _maxClients; i++)
		{
			_clientHandlers[i].Disconnect();
		}
	}
	_handlersMutex.unlock();

    _threadShouldStop = true;
    _thread.join();
	_threadRunning = false;

    _sendMutex.lock();
    {
        while(_messagesToSend.empty())
        {
            _messagesToSend.pop();
        }
    }
    _sendMutex.unlock();

    _socket.Disconnect();

    std::vector<std::unique_ptr<IMessage>> unprocessedMessages;
    _receivedMutex.lock();
    {
        unprocessedMessages.push_back(move(_receivedMessages.front()));
        _receivedMessages.pop();
    }
    _receivedMutex.unlock();

    return unprocessedMessages;
}

void ServerConnection::Run()
{
	Logging::Log("ServerConnection", "Running Server Connection");
	Socket clientSocket;

    while (!_threadShouldStop)
    {
		if (!clientSocket.Active())
		{
			clientSocket = _socket.AcceptSocket();
		}
		else
		{
			Logging::Log("ServerConnection", "Client Full, Not Accepting Any New Client");
		}

		if (clientSocket.Active())
		{
			Logging::Log("ServerConnection", "Handling Accepted Client");
			HandleClient(clientSocket);
		}

		std::shared_ptr<IMessage> nextMessage = nullptr;
		_sendMutex.lock();
		{
			nextMessage = _messagesToSend.empty() ? nullptr : move(_messagesToSend.front());

			if (nextMessage.get() != nullptr)
			{
				_messagesToSend.pop();
			}
		}
		_sendMutex.unlock();

		if (nextMessage.get() != nullptr)
		{
			std::shared_ptr<std::string> message = dynamic_cast<StringMessage*>(nextMessage.get())->AsType();
			Logging::Log("ServerConnection", "Sending Message: " + *message);

			for (int i = 0; i < _maxClients; i++)
			{
				_clientHandlers[i].SendMessage(nextMessage);
			}
		}

		for (int i = 0; i < _maxClients; i++)
		{
			std::unique_ptr<IMessage> clientMessage = _clientHandlers[i].GetMessage();
			if (clientMessage.get() != nullptr)
			{
				_receivedMutex.lock();
				{
					_receivedMessages.push(move(clientMessage));
				}
				_receivedMutex.unlock();

				// should be removed at some point
				std::unique_ptr<StringByteFormatter> stringFormatter = std::make_unique<StringByteFormatter>();
				SendMessage(std::make_unique<StringMessage>(move(stringFormatter), "Message Received"));
			}
		}
    }
}

void ServerConnection::HandleClient(Socket clientSocket)
{
	_handlersMutex.lock();
	{
		for (int i = 0; i < _maxClients; i++)
		{
			if (!_clientHandlers[i].Active())
			{
				clientSocket.SetBlocking(_socket.GetBlocking());
				_clientHandlers[i].SetSocket(clientSocket);
				_clientHandlers[i].Connect();
				break;
			}
		}
	}
	_handlersMutex.unlock();
}

ServerConnection::ClientHandler::ClientHandler()
{}

ServerConnection::ClientHandler::ClientHandler(Socket socket)
	: _socket(socket)
{}

ServerConnection::ClientHandler::~ClientHandler()
{
	Disconnect();
}

bool ServerConnection::ClientHandler::Active()
{
	return _socket.Active();
}

void ServerConnection::ClientHandler::SetSocket(Socket socket)
{
	if (socket.Active())
	{
		Disconnect();
	}

	_socket = socket;
}

std::unique_ptr<IMessage> ServerConnection::ClientHandler::GetMessage()
{
	std::unique_ptr<IMessage> oldestMessage = nullptr;

	_receivedMutex.lock();
	{
		oldestMessage = (_receivedMessages.size() == 0) ? nullptr : move(_receivedMessages.front());

		if (oldestMessage != nullptr)
		{
			_receivedMessages.pop();
		}
	}
	_receivedMutex.unlock();

	return oldestMessage;
}

void ServerConnection::ClientHandler::SendMessage(std::shared_ptr<IMessage> message)
{
	_sendMutex.lock();
	{
		_messagesToSend.push(move(message));
	}
	_sendMutex.unlock();
}

void ServerConnection::ClientHandler::Connect()
{
	_threadShouldStop = false;
	_thread = std::thread(&ServerConnection::ClientHandler::Run, this);
	_threadRunning = true;
}

void ServerConnection::ClientHandler::Disconnect()
{
	_threadShouldStop = true;
	_thread.join();
	_threadRunning = false;

	_socket.Disconnect();
}

void ServerConnection::ClientHandler::Run()
{
	Logging::Log("ServerConnection::ClientHandler", "Running Client Handler");
	std::vector<std::byte> messageReceived(1024);
	messageReceived.reserve(1024);

	while (!_threadShouldStop)
	{
		int byteReceived = _socket.Receive(messageReceived);

		if (byteReceived > 0)
		{
			Logging::Log("ServerConnection::ClientHandler", "Bytes Received: " + byteReceived);

			std::vector<std::byte> minimizedMessage = messageReceived;
			minimizedMessage.resize(byteReceived);

			std::unique_ptr<StringByteParser> stringParser = std::make_unique<StringByteParser>();
			std::unique_ptr<StringMessage> message = std::make_unique<StringMessage>(move(stringParser), minimizedMessage);

			Logging::Log("ServerConnection::ClientHandler", "Received Message: " + *(message->AsType()));

			_receivedMutex.lock();
			{
				_receivedMessages.push(move(message));
			}
			_receivedMutex.unlock();
		}

		std::shared_ptr<IMessage> nextMessage = nullptr;
		_sendMutex.lock();
		{
			nextMessage = _messagesToSend.empty() ? nullptr : move(_messagesToSend.front());

			if (nextMessage.get() != nullptr)
			{
				_messagesToSend.pop();
			}
		}
		_sendMutex.unlock();

		if (nextMessage.get() != nullptr)
		{
			std::shared_ptr<std::string> message = dynamic_cast<StringMessage*>(nextMessage.get())->AsType();
			Logging::Log("ServerConnection::ClientHandler", "Sending Message: " + *message);

			if (_socket.Send(nextMessage->AsBytes()) == -1)
			{
				Logging::Log("ServerConnection::ClientHandler", "Failed To Send Message");
			}
		}
	}
}