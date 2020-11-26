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
        std::unique_ptr<IMessage> GetMessage() override;
        void SendMessage(std::unique_ptr<IMessage> message) override;

        void Connect(Socket socket, Endpoint endpoint) override;
        void Connect(Endpoint endpoint) override;

        std::vector<std::unique_ptr<IMessage>> Disconnect(bool flush = false) override;

    private:
        Socket _socket;
        std::atomic<bool> _threadRunning = false;
        std::atomic<bool> _threadShouldStop = false;
        std::thread _thread;
        
        std::mutex _sendMutex;
        std::queue<std::unique_ptr<IMessage>> _messagesToSend;
        std::mutex _receivedMutex;
        std::queue<std::unique_ptr<IMessage>> _receivedMessages;

        void Run();
};