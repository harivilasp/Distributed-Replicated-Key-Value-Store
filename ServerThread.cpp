#include <iostream>
#include <memory>
#include <utility>

#include "ServerThread.h"
#include "ServerStub.h"

LaptopInfo LaptopFactory::
	CreateRegularLaptop(LaptopOrder order, int engineer_id)
{
	LaptopInfo laptop;
	laptop.CopyOrder(order);
	laptop.SetEngineerId(engineer_id);
	laptop.SetExpertId(-1);
	return laptop;
}

LaptopInfo LaptopFactory::
	CreateCustomLaptop(LaptopOrder order, int engineer_id)
{
	LaptopInfo laptop;
	laptop.CopyOrder(order);
	laptop.SetEngineerId(engineer_id);

	std::promise<LaptopInfo> prom;
	std::future<LaptopInfo> fut = prom.get_future();

	std::unique_ptr<ExpertRequest> req =
		std::unique_ptr<ExpertRequest>(new ExpertRequest);
	req->laptop = laptop;
	req->prom = std::move(prom);

	erq_lock.lock();
	erq.push(std::move(req));
	erq_cv.notify_one();
	erq_lock.unlock();

	laptop = fut.get();
	return laptop;
}

void LaptopFactory::
	EngineerThread(std::unique_ptr<ServerSocket> socket, int id)
{
	bool is_backup_node = false;
	std::cout << "EngineerThread: id = " << id << std::endl;
	int engineer_id = id;
	int laptop_type;
	LaptopOrder order;
	LaptopInfo laptop;

	ServerStub stub;
	Record record;
	stub.Init(std::move(socket));

	while (true)
	{
		if (is_backup_node) // role of back up node
		{
			std::cout << "EngineerThread: is_backup_node = true" << std::endl;
			std::cout << "EngineerThread: last_index = " << last_index << std::endl;
			std::cout << "EngineerThread: committed_index = " << committed_index << std::endl;
			std::cout << "EngineerThread: smr_log.size() = " << smr_log.size() << std::endl;
			std::cout << "EngineerThread: customer_record.size() = " << customer_record.size() << std::endl;

			ReplicaRequest replicaRequest = stub.ReceiveReplicaRequest();
			if (!replicaRequest.IsValid())
			{
				std::cout << "Connection broken back up node" << std::endl;
				is_backup_node = false;
				continue;
			}
			// request
			std::cout << "EngineerThread: replicaRequest.GetFactoryId() = " << replicaRequest.GetFactoryId() << std::endl;
			std::cout << "EngineerThread: replicaRequest.GetCommittedIndex() = " << replicaRequest.GetCommittedIndex() << std::endl;
			std::cout << "EngineerThread: replicaRequest.GetLastIndex() = " << replicaRequest.GetLastIndex() << std::endl;
			std::cout << "EngineerThread: replicaRequest.GetMapOp().arg1 = " << replicaRequest.GetMapOp().arg1 << std::endl;
			std::cout << "EngineerThread: replicaRequest.GetMapOp().arg2 = " << replicaRequest.GetMapOp().arg2 << std::endl;
			// process request

			if (replicaRequest.GetCommittedIndex() > last_index)
			{
				smr_log.push_back(replicaRequest.GetMapOp());
				int primary_committed_index = replicaRequest.GetCommittedIndex();
				for (int i = last_index + 1; i <= primary_committed_index; i++)
				{
					// MapOp op = smr_log[i];
					// customer_record[op.arg1] = op.arg2;
					std::cout << "Applyging map op to customer record" << std::endl;
				}
				last_index = replicaRequest.GetLastIndex();
				committed_index = primary_committed_index;
			}
			std::cout << "EngineerThread: last_index = " << last_index << std::endl;
			std::cout << "EngineerThread: committed_index = " << committed_index << std::endl;

			ReplicaResponse response;
			response.SetStatus(1);
			stub.SendReplicaResponse(response);
			continue;
		}
		std::cout << "EngineerThread: before processing" << std::endl;
		order = stub.ReceiveOrder();
		if (!order.IsValid())
		{
			std::cout << "Connection broken engineer" << std::endl;
			break;
		}
		laptop_type = order.GetLaptopType();
		std::cout << "EngineerThread: processing laptop_type = " << laptop_type << std::endl;
		switch (laptop_type)
		{
		case 0:
			laptop = CreateRegularLaptop(order, engineer_id);
			stub.SendLaptop(laptop);
			break;
		case 1:
			laptop = CreateCustomLaptop(order, engineer_id);
			stub.SendLaptop(laptop);
			break;
		case 3:
			cr_lock.lock();
			if (customer_record.find(order.GetCustomerId()) != customer_record.end())
			{
				int last_ind = customer_record[order.GetCustomerId()];
				record.SetRecord(order.GetOrderNumber(), last_ind);
			}
			else
			{
				record.SetRecord(order.GetOrderNumber(), -1);
			}
			stub.ReturnRecord(record);
			record.Print();
			cr_lock.unlock();
			break;
		case 4:
			std::cout << "Special order recieved setting back up node to true" << std::endl;
			{
				is_backup_node = true;
				stub.SendLaptop(laptop);
			}
			break;
		default:
			std::cout << "Undefined laptop type: "
					  << laptop_type << std::endl;
		}
	}
}

void LaptopFactory::ExpertThread(int id)
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
		smr_lock.lock();
		cr_lock.lock();
		std::cout << "special engineer thread smr_lock locked" << std::endl;
		smr_log.push_back({1, req->laptop.GetCustomerId(), req->laptop.GetEngineerId()});
		customer_record[req->laptop.GetCustomerId()] = smr_log.size() - 1;
		std::cout << "smr_log size: " << smr_log.size() << std::endl;
		if (replicas_connections_made == false)
		{
			MakeReplicaConnections();
			replicas_connections_made = true;
		}
		std::cout << "finished creating replicas" << std::endl;
		ReplicaRequest request;
		request.SetRequest(factory_id, smr_log.size() - 1, smr_log.size() - 1, {1, 1, 1});
		for (auto &replica : replica_stubs)
		{
			std::cout << "sending replica request to replica" << std::endl;
			ReplicaResponse response = replica->SendReplicaRequest(request);
			std::cout << "recieved confirmation from replica " << response.GetStatus() << std::endl;
		}
		cr_lock.unlock();
		smr_lock.unlock();
		// std::this_thread::sleep_for(std::chrono::microseconds(100));
		req->laptop.SetExpertId(id);
		req->prom.set_value(req->laptop);
	}
}

void LaptopFactory::MakeReplicaConnections()
{
	std::cout << "Making replica connections" << std::endl;
	if (replica_stubs.size() == 0)
	{
		for (auto &replica : replicas)
		{
			std::unique_ptr<ServerClientStub> stub(new ServerClientStub());
			stub->Init(replica.first, replica.second);
			std::cout << "Made connection to " << replica.first << ":" << replica.second << std::endl;
			LaptopOrder order;
			order.SetOrder(0, 0, 4);
			stub->OrderLaptop(order);
			std::cout << "Registration Order sent to replica" << std::endl;
			replica_stubs.emplace_back(std::move(stub)); // Move the unique_ptr into the vector
		}
	}
}

LaptopFactory::LaptopFactory()
{
	last_index = -1;
	committed_index = -1;
	primary_id = -1;
	factory_id = -1;
	std::pair<std::string, int> replica = {"127.0.0.1", 12346};
	replicas.push_back(replica);
}