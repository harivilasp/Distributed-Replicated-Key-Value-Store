#include <iostream>
#include "ServerStub.h"

ServerStub::ServerStub() {}

void ServerStub::Init(std::unique_ptr<ServerSocket> socket)
{
	this->socket = std::move(socket);
}

LaptopOrder ServerStub::ReceiveOrder()
{
	char buffer[32];
	LaptopOrder order;
	if (socket->Recv(buffer, order.Size(), 0))
	{
		order.Unmarshal(buffer);
		std::cout << "ServerStub::ReceiveOrder: order.Size() = " << order.Size() << std::endl;
	}
	return order;
}

int ServerStub::SendLaptop(LaptopInfo info)
{
	char buffer[32];
	info.Marshal(buffer);
	std::cout << "ServerStub::SendLaptop: info.Size() = " << info.Size() << std::endl;
	return socket->Send(buffer, info.Size(), 0);
}

int ServerStub::ReturnRecord(Record record)
{
	char buffer[32];
	record.Marshal(buffer);
	std::cout << "ServerStub::ReturnRecord: record.Size() = " << record.Size() << std::endl;
	return socket->Send(buffer, record.Size(), 0);
}

ReplicaRequest ServerStub::ReceiveReplicaRequest()
{
	ReplicaRequest replicaRequest;
	char buffer[32];
	if (socket->Recv(buffer, replicaRequest.Size(), 0))
	{
		replicaRequest.Unmarshal(buffer);
		std::cout << "ServerStub::ReceiveReplicaRequest: replicaRequest.Size() = " << replicaRequest.Size() << std::endl;
	}
	return replicaRequest;
}

int ServerStub::SendReplicaResponse(ReplicaResponse replicaResponse)
{
	char buffer[32];
	replicaResponse.Marshal(buffer);
	std::cout << "ServerStub::SendReplicaResponse: replicaResponse.Size() = " << replicaResponse.Size() << std::endl;
	return socket->Send(buffer, replicaResponse.Size(), 0);
}