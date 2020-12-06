#include "ServerConnection.h"

// testing
#include <iostream>

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

void ServerConnection::Connect(Socket socket, Endpoint endpoint)
{
    if (_threadRunning)
    {
        Disconnect();
    }

    _socket = socket;
    Connect(endpoint);
}

void ServerConnection::Connect(Endpoint endpoint)
{
    if (_threadRunning)
    {
        Disconnect();
    }

    _socket.Bind(endpoint);
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
	std::cout << "running server connection" << std::endl;
	Socket clientSocket;

    while (!_threadShouldStop)
    {
		if (clientSocket.Active())
		{
			clientSocket = _socket.AcceptSocket();
			std::cout << "client accepted" << std::endl;
		}
		if (clientSocket.Active())
		{
			std::cout << "no client accepted" << std::endl;
		}
		else
		{
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
			std::cout << "sending message: " << *message << std::endl;

			for (int i = 0; i < _maxClients; i++)
			{
				_clientHandlers[i].SendMessage(nextMessage);
			}
		}
		else
		{
			std::cout << "no message to send" << std::endl;
		}

		for (int i = 0; i < _maxClients; i++)
		{
			std::unique_ptr<IMessage> clientMessage = _clientHandlers[i].GetMessage();
			if (clientMessage.get() == nullptr)
			{
				_receivedMutex.lock();
				{
					_receivedMessages.push(clientMessage);
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
				_clientHandlers[i].SetSocket(clientSocket);
				_clientHandlers[i].Connect();
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
	_thread = std::thread(&ServerConnection::Run, this);
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
	std::cout << "running client handler" << std::endl;
	std::vector<std::byte> messageReceived(1024);
	messageReceived.reserve(1024);

	while (!_threadShouldStop)
	{
		int byteReceived = _socket.Receive(messageReceived);

		if (byteReceived > 0)
		{
			std::cout << "bytes received: " << byteReceived << std::endl;

			std::vector<std::byte> minimizedMessage = messageReceived;
			minimizedMessage.resize(byteReceived);

			std::unique_ptr<StringByteParser> stringParser = std::make_unique<StringByteParser>();
			std::unique_ptr<StringMessage> message = std::make_unique<StringMessage>(move(stringParser), minimizedMessage);

			std::cout << "received message: " << message->AsType() << std::endl;

			_receivedMutex.lock();
			{
				_receivedMessages.push(move(message));
			}
			_receivedMutex.unlock();
		}
		else
		{
			std::cout << "no message received" << std::endl;
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
			std::cout << "sending message: " << *message << std::endl;

			if (_socket.Send(nextMessage->AsBytes()) == -1)
			{
				std::cout << "failed to send message" << std::endl;
			}
		}
		else
		{
			std::cout << "no message to send" << std::endl;
		}
	}
}