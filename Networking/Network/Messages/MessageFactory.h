#pragma once

#include <cstddef>
#include <functional>
#include <map>
#include <memory>
#include <vector>

#include "WrappedMessage.h"
#include "StringMessage.h"
#include "ConnectMessage.h"
#include "DisconnectMessage.h"
#include "HeartbeatMessage.h"

namespace Message::Factory
{
	namespace
	{
		class Factory
		{
		public:
			Factory()
			{
				RegisterConstructor<WrappedMessage>();
				RegisterConstructor<StringMessage>();
				RegisterConstructor<ConnectMessage>();
				RegisterConstructor<DisconnectMessage>();
				RegisterConstructor<HeartbeatMessage>();
			}

			template <typename T>
			void RegisterConstructor()
			{
				// if we have a macro that creates classes with a specific static method, we would not need the below temp instance
				std::unique_ptr<T> temp = std::make_unique<T>();
				_messageConstructors[temp->GetMessageHash()] = [](std::vector<std::byte> bytes)
				{
					// we may want to just require that all IMessage types implement a constructor that takes in bytes to make this simpler
					std::unique_ptr<T> message = std::make_unique<T>();
					message->FromBytes(bytes);

					return move(message);
				};
			}

			template <typename T>
			std::unique_ptr<T> ConstructMessage(std::vector<std::byte> bytes)
			{
				std::unique_ptr<T> temp = std::make_unique<T>();
				return static_cast<std::unique_ptr<T>>(temp->GetMessageHashConstructMessage(), (bytes));
			}

			std::unique_ptr<IMessage> ConstructMessage(int hash, std::vector<std::byte> bytes)
			{
				// if we have a macro that creates classes with a specific static method, we would not need the below temp instance
				return _messageConstructors[hash](bytes);
			}

			std::unique_ptr<IMessage> ConstructMessage(MessageHeader header, std::vector<std::byte> bytes)
			{
				return _messageConstructors[header.GetTypeHash()](bytes);
			}

		private:
			// hash to function that creates the given message
			std::map<int, std::function<std::unique_ptr<IMessage>(std::vector<std::byte>)>> _messageConstructors;
		};

		Factory MESSAGE_FACTORY;
	}

	template <typename T>
	void RegisterConstructor()
	{
		MESSAGE_FACTORY.RegisterConstructor<T>();
	}

	template <typename T>
	std::unique_ptr<T> ConstructMessage(std::vector<std::byte> bytes)
	{
		return MESSAGE_FACTORY.ConstructMessage<T>(bytes);
	}

	std::unique_ptr<IMessage> ConstructMessage(int hash, std::vector<std::byte> bytes)
	{
		return MESSAGE_FACTORY.ConstructMessage(hash, bytes);
	}

	std::unique_ptr<IMessage> ConstructMessage(MessageHeader header, std::vector<std::byte> bytes)
	{
		return MESSAGE_FACTORY.ConstructMessage(header, bytes);
	}
}