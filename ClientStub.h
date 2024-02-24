#ifndef __CLIENT_STUB_H__
#define __CLIENT_STUB_H__

#include <string>

#include "ClientSocket.h"
#include "Messages.h"

class ClientStub
{
private:
	ClientSocket socket;

public:
	ClientStub();
	int Init(std::string ip, int port);
	LaptopInfo OrderLaptop(CustomerRequest order);
	CustomerRecord ReadRecord(CustomerRequest order);
	ReplicaResponse SendReplicaRequest(ReplicaRequest replicaRequest);
};

#endif // end of #ifndef __CLIENT_STUB_H__
