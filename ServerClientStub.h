#ifndef __CLIENT_STUB_H__
#define __CLIENT_STUB_H__

#include <string>

#include "ServerClientSocket.h"
#include "Messages.h"

class ServerClientStub
{
private:
	ServerClientSocket socket;

public:
	ServerClientStub();
	virtual ~ServerClientStub();
	int Init(std::string ip, int port);
	LaptopInfo OrderLaptop(CustomerRequest order);
	CustomerRecord ReadRecord(CustomerRequest order);
	ReplicaResponse SendReplicaRequest(ReplicaRequest replicaRequest);
};

#endif // end of #ifndef __CLIENT_STUB_H__
