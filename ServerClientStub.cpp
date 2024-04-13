#include "ServerClientStub.h"
#include <iostream>

ServerClientStub::ServerClientStub(){};

int ServerClientStub::Init(std::string ip, int port)
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

LaptopInfo ServerClientStub::OrderLaptop(CustomerRequest order)
{
	LaptopInfo info;
	char buffer[32];
	int size;
	order.Marshal(buffer);
	size = order.Size();
//	std::cout << "ServerClientStub::OrderLaptop: order.Size() = " << size << std::endl;
	order.Print();
//	std::cout << "ServerClientStub::OrderLaptop: order printed" << std::endl;
	if (socket.Send(buffer, size, 0))
	{
//		std::cout << "ServerClientStub::OrderLaptop: sent order" << std::endl;
		size = info.Size();
//		std::cout << "ServerClientStub::OrderLaptop: waiting for laptopinfo = " << std::endl;
		if (socket.Recv(buffer, size, 0))
		{
			info.Unmarshal(buffer);
		}
	}
	return info;
}

CustomerRecord ServerClientStub::ReadRecord(CustomerRequest order)
{
	CustomerRecord record;
	char buffer[32];
	int size;
	order.Marshal(buffer);
	size = order.Size();
	if (socket.Send(buffer, size, 0))
	{
//		std::cout << "ServerClientStub::ReadRecord: sent order" << std::endl;
		size = record.Size();
//		std::cout << "ServerClientStub::ReadRecord: waiting for record = " << std::endl;
		if (socket.Recv(buffer, size, 0))
		{
//			std::cout << "ServerClientStub::ReadRecord: received record" << std::endl;
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
	// std::cout << "ServerClientStub::SendReplicaRequest: replicaRequest.Size() = " << size << std::endl;
	// replicaRequest.Print();
	// std::cout << "ServerClientStub::SendReplicaRequest: replicaRequest printed" << std::endl;
	ReplicaResponse response;
	if (socket.Send(buffer, size, 0))
	{
		// std::cout << "ServerClientStub::SendReplicaRequest: sent request" << std::endl;
		size = response.Size();
		// std::cout << "ServerClientStub::SendReplicaRequest: waiting for response = " << std::endl;
		if (socket.Recv(buffer, size, 0))
		{
			response.Unmarshal(buffer);
			// std::cout << "ServerClientStub::SendReplicaRequest: recived response" << std::endl;
		}
	}
	return response;
}

ServerClientStub::~ServerClientStub()
{
	// std::cout << "ServerClientStub::~ServerClientStub: deleting stub" << std::endl;
}