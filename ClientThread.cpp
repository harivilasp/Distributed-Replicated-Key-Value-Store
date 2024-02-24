#include "ClientThread.h"
#include "Messages.h"

#include <iostream>

ClientThreadClass::ClientThreadClass() {}

void ClientThreadClass::
	ThreadBody(std::string ip, int port, int id, int orders, int type)
{
	customer_id = id;
	num_orders = orders;
	request_type = type;
	if (!stub.Init(ip, port))
	{
		std::cout << "Thread " << customer_id << " failed to connect" << std::endl;
		return;
	}
	if (request_type == 3)
	{
		for (int i = 0; i < 3; i++)
		{
			CustomerRequest customerRequest;
			CustomerRecord customerRecord;
			customerRequest.SetCustomerRequest(customer_id, i, request_type);
			customerRecord = stub.ReadRecord(customerRequest);
			std::cout << "Thread " << customer_id << " customerRecord " << i << std::endl;
			customerRecord.Print();
		}
		return;
	}

	for (int i = 0; i < num_orders; i++)
	{
		CustomerRequest customerRequest;
		LaptopInfo laptop;
		std::cout << "Thread " << customer_id << " customerRequest " << i << std::endl;
		customerRequest.SetCustomerRequest(customer_id, i, request_type);

		timer.Start();
		laptop = stub.OrderLaptop(customerRequest);
		timer.EndAndMerge();

		if (!laptop.IsValid())
		{
			std::cout << "Invalid laptop " << customer_id << std::endl;
			break;
		}
	}
}

ClientTimer ClientThreadClass::GetTimer()
{
	return timer;
}
