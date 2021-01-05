#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <thread>
#include <queue>

#include "SocketConnection.h"

class ServerConnection : public IConnection, public IReceiver, public ISender
{
	class ClientHandler
	{
	public:
		ClientHandler();
		ClientHandler(Socket socket);
		~ClientHandler();

		bool Active();

		void SetSocket(Socket socket);

		std::unique_ptr<Message::IMessage> GetMessage();
		void SendMessage(std::shared_ptr<Message::IMessage> message);

		void Connect();
		void Disconnect();

	private:
		Socket _socket;
		std::atomic<bool> _threadRunning = false;
		std::atomic<bool> _threadShouldStop = false;
		std::thread _thread;

		std::mutex _sendMutex;
		std::queue<std::shared_ptr<Message::IMessage>> _messagesToSend;

		std::mutex _receivedMutex;
		std::queue<std::unique_ptr<Message::IMessage>> _receivedMessages;

		void Run();
	};

    public:
		~ServerConnection();

        std::unique_ptr<Message::IMessage> GetMessage() override;
        void SendMessage(std::unique_ptr<Message::IMessage> message) override;

		void SetMaxClients(int maxClients);

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

        std::mutex _handlersMutex;
		int _maxClients;
		// this will require a destructor to free the memory for the array
        std::unique_ptr<ClientHandler[]> _clientHandlers;

        void Run();
		void HandleClient(Socket clientSocket);
};