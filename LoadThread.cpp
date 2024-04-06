#include <iostream>
#include <memory>
#include <utility>

#include "LoadThread.h"
#include "ServerStub.h"
#include "LoadServerStub.h"
#include "LoadServerClientStub.h"

LaptopInfo LoadFactory::
	CreateRegularLaptop(CustomerRequest customerRequest, int engineer_id)
{
	LaptopInfo laptop;
	laptop.CopyOrder(customerRequest);
	laptop.SetEngineerId(engineer_id);
	laptop.SetExpertId(-1);
	return laptop;
}

LaptopInfo LoadFactory::
	CreateCustomLaptop(CustomerRequest customerRequest, int engineer_id)
{
	LaptopInfo laptop;
	laptop.CopyOrder(customerRequest);
	laptop.SetEngineerId(engineer_id);

	std::promise<LaptopInfo> prom;
	std::future<LaptopInfo> fut = prom.get_future();

	std::unique_ptr<LoadRequest> req =
		std::unique_ptr<LoadRequest>(new LoadRequest);
	req->laptop = laptop;
	req->prom = std::move(prom);

	erq_lock.lock();
	erq.push(std::move(req));
	erq_cv.notify_one();
	erq_lock.unlock();

	laptop = fut.get();
	return laptop;
}

void LoadFactory::
	EngineerThread(std::unique_ptr<ServerSocket> socket, int id)
{
	int request_type;
	CustomerRequest customerRequest;
	LaptopInfo laptop;

	ServerStub stub;
	std::unique_ptr<ServerClientStub> client_stub;
	client_stub = connect_server(id);
	CustomerRecord record;
	stub.Init(std::move(socket));
	while (true)
	{
		customerRequest = stub.ReceiveRequest();
		if (!customerRequest.IsValid())
		{
			// std::cout << "Connection broken engineer" << std::endl;
			break;
		}
		request_type = customerRequest.GetLaptopType();
		switch (request_type)
		{
		case 1:
			laptop = client_stub->OrderLaptop(customerRequest);
			// set in cache and send back to client
			stub.SendLaptop(laptop);
			break;
		case 2: // read for one customer id
			cr_lock.lock();
			if (customer_record_cache.find(customerRequest.GetCustomerId()) != customer_record_cache.end())
			{
				std::cout << "LoadThread server:: Read record from cache " << id << std::endl;
				// get from cache
				int last_ind = customer_record_cache[customerRequest.GetCustomerId()];
				record.SetRecord(customerRequest.GetCustomerId(), last_ind);
				std::this_thread::sleep_for(std::chrono::microseconds(10));
			}
			else
			{
				CustomerRecord temp = client_stub->ReadRecord(customerRequest);
				record.SetRecord(customerRequest.GetCustomerId(), temp.GetLastOrder());
				std::cout << "LoadThread server:: Read record from server " << id << std::endl;
				temp.Print();
				// also set in cache
				if (temp.GetLastOrder() != -1)
				{
					customer_record_cache[customerRequest.GetCustomerId()] = temp.GetLastOrder();
				}
			}
			stub.ReturnRecord(record);
			// record.Print();
			cr_lock.unlock();
			break;
		default:
			std::cout << "Undefined laptop type: "
					  << request_type << std::endl;
		}
	}
}

void LoadFactory::ExpertThread(int id)
{
	std::unique_lock<std::mutex> ul(erq_lock, std::defer_lock);
	while (true)
	{
		ul.lock();

		if (erq.empty())
		{
			erq_cv.wait(ul, [this]
						{ return !erq.empty(); });
		}

		auto req = std::move(erq.front());
		erq.pop();

		ul.unlock();
		// std::this_thread::sleep_for(std::chrono::microseconds(100));
		req->laptop.SetExpertId(id);
		req->prom.set_value(req->laptop);
	}
}

LoadFactory::LoadFactory()
{
	last_index = -1;
	committed_index = -1;
	primary_id = -1;
}

void LoadFactory::AddReplica(int id, std::string ip, int port)
{
	std::pair<std::string, int> replica = {ip, port};
	replicas.push_back(replica);
	replica_id_to_ip_port[id] = replica;
}

std::unique_ptr<ServerClientStub> LoadFactory::connect_server(int id)
{
	// connect with suitable server in round robin manner
	std::lock_guard<std::mutex> guard(round_robin_lock);
	bool connected = false;
	int counter = 0;
	std::unique_ptr<ServerClientStub> stub = std::unique_ptr<ServerClientStub>(new ServerClientStub());
	while (!connected && counter < replicas.size())
	{
		int serverNumber = round_robin_counter % replicas.size();
		if (stub->Init(replicas[serverNumber].first, replicas[serverNumber].second))
		{
			std::cout << "LoadThread:: connected to server " << id << replicas[serverNumber].first << replicas[serverNumber].second << " : " << round_robin_counter << std::endl;
			connected = true;
			break;
		}
		counter++;
		round_robin_counter++;
	}
	round_robin_counter++;
	if (counter >= replicas.size())
	{
		std::cout << "LoadThread:: Could not connect to any server" << std::endl;
		// return stub;
	}
	return stub;
}