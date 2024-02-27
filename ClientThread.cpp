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
		for (int i = 0; i < num_orders; i++)
		{
			CustomerRequest customerRequest;
			CustomerRecord customerRecord;
			customerRequest.SetCustomerRequest(i, i, 2);
			customerRecord = stub.ReadRecord(customerRequest);
			// std::cout << "Thread " << i << " customerRecord" << std::endl;
			if (customerRecord.GetLastOrder() != -1)
				customerRecord.Print();
		}
		return;
	}
	if (request_type == 2) // read for one customer id
	{
		CustomerRequest customerRequest;
		CustomerRecord customerRecord;
		customerRequest.SetCustomerRequest(customer_id, 0, request_type);
		customerRecord = stub.ReadRecord(customerRequest);
		// std::cout << "Thread " << customer_id << " customerRecord " << 0 << std::endl;
		customerRecord.Print();
		return;
	}

	for (int i = 0; i < num_orders; i++)
	{
		CustomerRequest customerRequest;
		LaptopInfo laptop;
		// std::cout << "Thread " << customer_id << " customerRequest " << i << std::endl;
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
