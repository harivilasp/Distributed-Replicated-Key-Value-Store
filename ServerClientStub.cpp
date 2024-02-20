#include "ServerClientStub.h"
#include <iostream>

ServerClientStub::ServerClientStub() {}

int ServerClientStub::Init(std::string ip, int port)
{
	return socket.Init(ip, port);
}

LaptopInfo ServerClientStub::OrderLaptop(LaptopOrder order)
{
	LaptopInfo info;
	char buffer[32];
	int size;
	order.Marshal(buffer);
	size = order.Size();
	if (socket.Send(buffer, size, 0))
	{
		size = info.Size();
		if (socket.Recv(buffer, size, 0))
		{
			info.Unmarshal(buffer);
		}
	}
	return info;
}

Record ServerClientStub::ReadRecord(LaptopOrder order)
{
	Record record;
	char buffer[32];
	int size;
	order.Marshal(buffer);
	size = order.Size();
	if (socket.Send(buffer, size, 0))
	{
		std::cout << "ClientStub::ReadRecord: sent order" << std::endl;
		// memset(buffer, 0, 32);
		size = record.Size();
		if (socket.Recv(buffer, size, 0))
		{
			std::cout << "ClientStub::ReadRecord: received record" << std::endl;
			record.Unmarshal(buffer);
		}
	}
	return record;
}

ReplicaResponse ServerClientStub::SendReplicaRequest(ReplicaRequest replicaRequest)
{
	char buffer[32];
	replicaRequest.Marshal(buffer);
	int size = replicaRequest.Size();
	ReplicaResponse response;
	if (socket.Send(buffer, size, 0))
	{
		size = response.Size();
		if (socket.Recv(buffer, size, 0))
		{
			response.Unmarshal(buffer);
		}
	}
	return response;
}