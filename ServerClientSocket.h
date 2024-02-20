#ifndef __SERVERCLIENTSOCKET_H__
#define __SERVERCLIENTSOCKET_H__

#include <string>

#include "Socket.h"

class ServerClientSocket : public Socket
{
public:
	ServerClientSocket() {}
	~ServerClientSocket() {}

	int Init(std::string ip, int port);
};

#endif // end of #ifndef __CLIENTSOCKET_H__
