#pragma once

#include <stdint.h>

namespace Message
{
	class ConnectionData
	{
	public:
		ConnectionData()
			: _clientID(0)
			, _serverID(0)
		{}

		ConnectionData(uint32_t clientID, uint32_t serverID)
			: _clientID(clientID)
			, _serverID(serverID)
		{}

		ConnectionData(const ConnectionData& other)
			: _clientID(other._clientID)
			, _serverID(other._serverID)
		{}

		uint32_t GetClientID()
		{
			return _clientID;
		}

		uint32_t GetServerID()
		{
			return _serverID;
		}

	private:
		uint32_t _clientID;
		uint32_t _serverID;
	};
}