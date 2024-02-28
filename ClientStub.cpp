#include "ClientStub.h"
#include <iostream>

ClientStub::ClientStub() {}

int ClientStub::Init(std::string ip, int port)
{
	try
	{
		return socket.Init(ip, port);
	}
	catch (const char *msg)
	{
		// std::cerr << msg << std::endl;
		return 0;
	}
}

LaptopInfo ClientStub::OrderLaptop(CustomerRequest order)
{
	LaptopInfo info;
	char buffer[32];
	int size;
	order.Marshal(buffer);
	size = order.Size();
	if (socket.Send(buffer, size, 0))
	{
		// std::cout << "ClientStub::OrderLaptop: sent order" << std::endl;
		size = info.Size();
		// std::cout << "ClientStub::OrderLaptop: waiting for laptopinfo = " << std::endl;
		if (socket.Recv(buffer, size, 0))
		{
			info.Unmarshal(buffer);
		}
	}
	return info;
}

CustomerRecord ClientStub::ReadRecord(CustomerRequest order)
{
	CustomerRecord record;
	char buffer[32];
	int size;
	order.Marshal(buffer);
	size = order.Size();
	if (socket.Send(buffer, size, 0))
	{
		// std::cout << "ClientStub::ReadRecord: sent order" << std::endl;
		size = record.Size();
		// std::cout << "ClientStub::ReadRecord: waiting for record = " << std::endl;
		if (socket.Recv(buffer, size, 0))
		{
			// std::cout << "ClientStub::ReadRecord: received record" << std::endl;
			record.Unmarshal(buffer);
		}
	}
	return record;
}

ReplicaResponse ClientStub::SendReplicaRequest(ReplicaRequest replicaRequest)
{
	char buffer[32];
	replicaRequest.Marshal(buffer);
	int size = replicaRequest.Size();
	ReplicaResponse response;
	if (socket.Send(buffer, size, 0))
	{
		// std::cout << "ClientStub::SendReplicaRequest: waiting for ReplicaResponse = " << std::endl;
		size = response.Size();
		if (socket.Recv(buffer, size, 0))
		{
			response.Unmarshal(buffer);
		}
	}
	return response;
}