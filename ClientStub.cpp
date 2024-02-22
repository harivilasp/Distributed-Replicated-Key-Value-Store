#include "ClientStub.h"
#include <iostream>

ClientStub::ClientStub() {}

int ClientStub::Init(std::string ip, int port)
{
	return socket.Init(ip, port);
}

LaptopInfo ClientStub::OrderLaptop(LaptopOrder order)
{
	LaptopInfo info;
	char buffer[32];
	int size;
	order.Marshal(buffer);
	size = order.Size();
	if (socket.Send(buffer, size, 0))
	{
		std::cout << "ClientStub::OrderLaptop: sent order" << std::endl;
		size = info.Size();
		std::cout << "ClientStub::OrderLaptop: waiting for laptopinfo = " << std::endl;
		if (socket.Recv(buffer, size, 0))
		{
			info.Unmarshal(buffer);
		}
	}
	return info;
}

Record ClientStub::ReadRecord(LaptopOrder order)
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
		std::cout << "ClientStub::ReadRecord: waiting for record = " << std::endl;
		if (socket.Recv(buffer, size, 0))
		{
			std::cout << "ClientStub::ReadRecord: received record" << std::endl;
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
		std::cout << "ClientStub::SendReplicaRequest: waiting for ReplicaResponse = " << std::endl;
		size = response.Size();
		if (socket.Recv(buffer, size, 0))
		{
			response.Unmarshal(buffer);
		}
	}
	return response;
}