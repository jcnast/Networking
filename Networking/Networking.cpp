// Networking.cpp : Defines the entry point for the application.
//

#include "Networking.h"

#ifdef CLIENT
#include "Client.h"
#endif

#ifdef SERVER
#include "Server.h"
#endif

using namespace std;

int main()
{
#ifdef CLIENT
	ExecuteClientConnection();
#endif

#ifdef SERVER
	ExecuteServerConnection();
#endif

	return 0;
}
