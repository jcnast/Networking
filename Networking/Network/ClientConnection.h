#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <thread>
#include <queue>

#include "SocketConnection.h"

class ClientConnection : public IConnection, public IReceiver, public ISender
{
    public:
        std::unique_ptr<Message::IMessage> GetMessage() override;
        void SendMessage(std::unique_ptr<Message::IMessage> message) override;

        void Connect(Socket socket, Endpoint endpoint, bool blocking) override;
        void Connect(Endpoint endpoint, bool blocking) override;

        std::vector<std::unique_ptr<Message::IMessage>> Disconnect(bool flush = false) override;

    private:
        Socket _socket;
        std::atomic<bool> _threadRunning = false;
        std::atomic<bool> _threadShouldStop = false;
        std::thread _thread;
        
        std::mutex _sendMutex;
        std::queue<std::unique_ptr<Message::IMessage>> _messagesToSend;
        std::mutex _receivedMutex;
        std::queue<std::unique_ptr<Message::IMessage>> _receivedMessages;

        void Run();
};