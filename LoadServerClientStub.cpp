#include "LoadServerClientStub.h"
#include <iostream>

LoadServerClientStub::LoadServerClientStub(){};

int LoadServerClientStub::Init(std::string ip, int port)
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

LaptopInfo LoadServerClientStub::OrderLaptop(CustomerRequest order)
{
	LaptopInfo info;
	char buffer[32];
	int size;
	order.Marshal(buffer);
	size = order.Size();
	// std::cout << "LoadServerClientStub::OrderLaptop: order.Size() = " << size << std::endl;
	// order.Print();
	// std::cout << "LoadServerClientStub::OrderLaptop: order printed" << std::endl;
	if (socket.Send(buffer, size, 0))
	{
		// std::cout << "LoadServerClientStub::OrderLaptop: sent order" << std::endl;
		size = info.Size();
		// std::cout << "LoadServerClientStub::OrderLaptop: waiting for laptopinfo = " << std::endl;
		if (socket.Recv(buffer, size, 0))
		{
			info.Unmarshal(buffer);
		}
	}
	return info;
}

CustomerRecord LoadServerClientStub::ReadRecord(CustomerRequest order)
{
	CustomerRecord record;
	char buffer[32];
	int size;
	order.Marshal(buffer);
	size = order.Size();
	if (socket.Send(buffer, size, 0))
	{
		// std::cout << "LoadServerClientStub::ReadRecord: sent order" << std::endl;
		size = record.Size();
		// std::cout << "LoadServerClientStub::ReadRecord: waiting for record = " << std::endl;
		if (socket.Recv(buffer, size, 0))
		{
			// std::cout << "LoadServerClientStub::ReadRecord: received record" << std::endl;
			record.Unmarshal(buffer);
		}
	}
	return record;
}

ReplicaResponse LoadServerClientStub::SendReplicaRequest(ReplicaRequest replicaRequest)
{
	char buffer[32];
	replicaRequest.Marshal(buffer);
	int size = replicaRequest.Size();
	// std::cout << "LoadServerClientStub::SendReplicaRequest: replicaRequest.Size() = " << size << std::endl;
	// replicaRequest.Print();
	// std::cout << "LoadServerClientStub::SendReplicaRequest: replicaRequest printed" << std::endl;
	ReplicaResponse response;
	if (socket.Send(buffer, size, 0))
	{
		// std::cout << "LoadServerClientStub::SendReplicaRequest: sent request" << std::endl;
		size = response.Size();
		// std::cout << "LoadServerClientStub::SendReplicaRequest: waiting for response = " << std::endl;
		if (socket.Recv(buffer, size, 0))
		{
			response.Unmarshal(buffer);
			// std::cout << "LoadServerClientStub::SendReplicaRequest: recived response" << std::endl;
		}
	}
	return response;
}

LoadServerClientStub::~LoadServerClientStub()
{
	// std::cout << "LoadServerClientStub::~LoadServerClientStub: deleting stub" << std::endl;
}