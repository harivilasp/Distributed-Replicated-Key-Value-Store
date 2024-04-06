#include <iostream>
#include "LoadServerStub.h"

LoadServerStub::LoadServerStub() {}

void LoadServerStub::Init(std::unique_ptr<ServerSocket> socket)
{
	this->socket = std::move(socket);
}

CustomerRequest LoadServerStub::ReceiveRequest()
{
	char buffer[32];
	CustomerRequest order;
	// std::cout << "LoadServerStub::ReceiveRequest: waiting for order" << std::endl;
	if (socket->Recv(buffer, order.Size(), 0))
	{
		order.Unmarshal(buffer);
		// std::cout << "LoadServerStub::ReceiveRequest : order.Size() = " << order.Size() << std::endl;
		// order.Print();
		// std::cout << "LoadServerStub::ReceiveRequest : order printed " << std::endl;
	}
	return order;
}

int LoadServerStub::SendLaptop(LaptopInfo info)
{
	char buffer[32];
	info.Marshal(buffer);
	// std::cout << "LoadServerStub::SendLaptop : info.Size() = " << info.Size() << std::endl;
	return socket->Send(buffer, info.Size(), 0);
}

int LoadServerStub::ReturnRecord(CustomerRecord record)
{
	char buffer[32];
	record.Marshal(buffer);
	// std::cout << "LoadServerStub::ReturnRecord : record.Size() = " << record.Size() << std::endl;
	return socket->Send(buffer, record.Size(), 0);
}

ReplicaRequest LoadServerStub::ReceiveReplicaRequest()
{
	ReplicaRequest replicaRequest;
	char buffer[32];
	// std::cout << "LoadServerStub::ReceiveReplicaRequest: waiting for replicaRequest" << std::endl;
	if (socket->Recv(buffer, replicaRequest.Size(), 0))
	{
		replicaRequest.Unmarshal(buffer);
		// std::cout << "LoadServerStub::ReceiveReplicaRequest : replicaRequest.Size() = " << replicaRequest.Size() << std::endl;
	}
	return replicaRequest;
}

int LoadServerStub::SendReplicaResponse(ReplicaResponse replicaResponse)
{
	char buffer[32];
	replicaResponse.Marshal(buffer);
	// std::cout << "LoadServerStub::SendReplicaResponse : replicaResponse.Size() = " << replicaResponse.Size() << std::endl;
	return socket->Send(buffer, replicaResponse.Size(), 0);
}