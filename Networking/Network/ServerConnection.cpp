#include "ServerConnection.h"

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

    _thread = std::thread(&ServerConnection::Run, this);
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
    std::vector<std::byte> messageReceived(1024);
    messageReceived.reserve(1024);

    while (!_threadShouldStop)
    {
        std::unique_ptr<ISocket> clientSocket = _socket.Accept();

        int byteReceived = _socket.Receive(messageReceived);

        if (byteReceived > 0)
        {
            std::vector<std::byte> minimizedMessage = messageReceived;
            minimizedMessage.resize(byteReceived);

            std::unique_ptr<StringByteParser> stringParser = std::make_unique<StringByteParser>();
            std::unique_ptr<StringMessage> message = std::make_unique<StringMessage>(move(stringParser), minimizedMessage);

            _receivedMutex.lock();
            {
                _receivedMessages.push(move(message));
            }
            _receivedMutex.unlock();

            std::unique_ptr<StringByteFormatter> stringFormatter = std::make_unique<StringByteFormatter>();
            SendMessage(std::make_unique<StringMessage>(move(stringFormatter), "Message Received"));
        }

        std::unique_ptr<IMessage> nextMessage = nullptr;
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
            _socket.Send(nextMessage->AsBytes());
        }

        clientSocket->Disconnect();
    }
}