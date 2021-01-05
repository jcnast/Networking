#include "ClientConnection.h"

#include "Network/Messages/StringMessage.h"

#include "Logging/Logger.h"

std::unique_ptr<Message::IMessage> ClientConnection::GetMessage()
{
    std::unique_ptr<Message::IMessage> oldestMessage = nullptr;

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

void ClientConnection::SendMessage(std::unique_ptr<Message::IMessage> message)
{
    _sendMutex.lock();
    {
        _messagesToSend.push(move(message));
    }
    _sendMutex.unlock();
}

void ClientConnection::Connect(Socket socket, Endpoint endpoint, bool blocking)
{
    if (_threadRunning)
    {
        Disconnect();
    }

    _socket = socket;
    Connect(endpoint, blocking);
}

void ClientConnection::Connect(Endpoint endpoint, bool blocking)
{
    if (_threadRunning)
    {
        Disconnect();
    }

    _socket.Connect(endpoint, blocking);

    _thread = std::thread(&ClientConnection::Run, this);
}

std::vector<std::unique_ptr<Message::IMessage>> ClientConnection::Disconnect(bool flush)
{
    while (flush)
    {
        _sendMutex.lock();
        {
            flush = _messagesToSend.empty();
        }
        _sendMutex.unlock();
    }

    _threadShouldStop = true;
    _thread.join();
    _threadShouldStop = false;

    _sendMutex.lock();
    {
        while(_messagesToSend.empty())
        {
            _messagesToSend.pop();
        }
    }
    _sendMutex.unlock();

    _socket.Disconnect();

    std::vector<std::unique_ptr<Message::IMessage>> unprocessedMessages;
    _receivedMutex.lock();
    {
        unprocessedMessages.push_back(move(_receivedMessages.front()));
        _receivedMessages.pop();
    }
    _receivedMutex.unlock();

    return unprocessedMessages;
}

void ClientConnection::Run()
{
	Logging::Log("ClientConnection", "Running Server Connection");
    std::vector<std::byte> messageReceived(1024);
    messageReceived.reserve(1024);

    while (!_threadShouldStop && _socket.Active())
    {
        std::unique_ptr<Message::IMessage> nextMessage = nullptr;
        _sendMutex.lock();
        {
            nextMessage = _messagesToSend.empty() ? nullptr : move(_messagesToSend.front());

            if (nextMessage != nullptr)
            {
                _messagesToSend.pop();
            }
        }
        _sendMutex.unlock();

        if (nextMessage.get() != nullptr)
        {
			std::shared_ptr<std::string> message = dynamic_cast<Message::StringMessage*>(nextMessage.get())->AsType();
			Logging::Log("ClientConnection", "Sending Message: " + *message);

			if (_socket.Send(nextMessage->AsBytes()) == -1)
			{
				Logging::Log("ClientConnection", "Failed To Send Message");
			}
        }

        int byteReceived = _socket.Receive(messageReceived);

        if (byteReceived > 0)
        {
            std::vector<std::byte> minimizedMessage = messageReceived;
            minimizedMessage.resize(byteReceived);

            std::unique_ptr<Message::StringMessage> message = std::make_unique<Message::StringMessage>(minimizedMessage);

			Logging::Log("ClientConnection", "Received Message: " + *(message->AsType()));

            _receivedMutex.lock();
            {
                _receivedMessages.push(move(message));
            }
            _receivedMutex.unlock();
        }
    }
}