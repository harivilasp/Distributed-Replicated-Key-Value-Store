#include "ClientThread.h"
#include "Messages.h"

#include <iostream>

ClientThreadClass::ClientThreadClass() {}

void ClientThreadClass::
	ThreadBody(std::string ip, int port, int id, int orders, int type, int num_customers)
{
	customer_id = id;
	num_orders = orders;
	request_type = type;
	if (!stub.Init(ip, port))
	{
		std::cout << "Primary server is not available" << std::endl;
		return;
	}
	if (request_type == 3)
	{
		for (int i = 0; i < num_orders; i++)
		{
            if((i % num_customers) == id)
            {
//                std::cout << "thread " << id << " :processing: " << i << std::endl;
                CustomerRequest customerRequest;
                CustomerRecord customerRecord;
                customerRequest.SetCustomerRequest(i, 0, 2);
                customerRecord = stub.ReadRecord(customerRequest);
                // std::cout << "Thread " << i << " customerRecord" << std::endl;
                if (customerRecord.GetLastOrder() != -1)
                    customerRecord.Print();
            }
		}
		return;
	}
    if (request_type == 4)
    {
        int j = num_orders, i = 0;
//        while(j < num_orders)
        {
            while (i <= j) {
                CustomerRequest customerRequest;
                CustomerRecord customerRecord;
                customerRequest.SetCustomerRequest(i, 0, 2);
                customerRecord = stub.ReadRecord(customerRequest);
                // std::cout << "Thread " << i << " customerRecord" << std::endl;
                if (customerRecord.GetLastOrder() != -1)
                    customerRecord.Print();
                i++;
            }
            while(i > 0) {
                i--;
                CustomerRequest customerRequest;
                CustomerRecord customerRecord;
                customerRequest.SetCustomerRequest(i, 0, 2);
                customerRecord = stub.ReadRecord(customerRequest);
                // std::cout << "Thread " << i << " customerRecord" << std::endl;
                if (customerRecord.GetLastOrder() != -1)
                    customerRecord.Print();
            }
//            j++;
        }
        return;
    }
	if (request_type == 2) // read for one customer id
	{
		CustomerRequest customerRequest;
		CustomerRecord customerRecord;
		customerRequest.SetCustomerRequest(orders, 0, request_type); // here orders is customer id
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
			// std::cout << "Invalid laptop " << customer_id << std::endl;
			break;
		}
	}
}

ClientTimer ClientThreadClass::GetTimer()
{
	return timer;
}
