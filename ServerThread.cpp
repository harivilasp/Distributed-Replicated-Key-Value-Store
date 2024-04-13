#include <iostream>
#include <memory>
#include <utility>

#include "ServerThread.h"
#include "ServerStub.h"

LaptopInfo LaptopFactory::
	CreateRegularLaptop(CustomerRequest customerRequest, int engineer_id)
{
	LaptopInfo laptop;
	laptop.CopyOrder(customerRequest);
	laptop.SetEngineerId(engineer_id);
	laptop.SetExpertId(-1);
	return laptop;
}

LaptopInfo LaptopFactory::
	CreateCustomLaptop(CustomerRequest customerRequest, int engineer_id)
{
	LaptopInfo laptop;
	laptop.CopyOrder(customerRequest);
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
//	std::cout << "EngineerThread: id = " << id << std::endl;
	int engineer_id = id;
	int request_type;
	CustomerRequest customerRequest;
	LaptopInfo laptop;

	ServerStub stub;
	CustomerRecord record;
	stub.Init(std::move(socket));

	while (true)
	{
		if (is_backup_node) // role of back up node
		{
			// std::cout << "EngineerThread: is_backup_node = true" << std::endl;
			// std::cout << "EngineerThread: last_index = " << last_index << std::endl;
			// std::cout << "EngineerThread: committed_index = " << committed_index << std::endl;
			// std::cout << "EngineerThread: smr_log.size() = " << smr_log.size() << std::endl;
			// std::cout << "EngineerThread: customer_record.size() = " << customer_record.size() << std::endl;

			ReplicaRequest replicaRequest = stub.ReceiveReplicaRequest();
			if (!replicaRequest.IsValid())
			{
				// std::cout << "Connection broken back up node" << std::endl;
				is_backup_node = false;
				primary_id = -1;
				continue;
			}
			// request
			// std::cout << "EngineerThread: replicaRequest.GetFactoryId() = " << replicaRequest.GetFactoryId() << std::endl;
			// std::cout << "EngineerThread: replicaRequest.GetCommittedIndex() = " << replicaRequest.GetCommittedIndex() << std::endl;
			// std::cout << "EngineerThread: replicaRequest.GetLastIndex() = " << replicaRequest.GetLastIndex() << std::endl;
			// std::cout << "EngineerThread: replicaRequest.GetMapOp().arg1 = " << replicaRequest.GetMapOp().arg1 << std::endl;
			// std::cout << "EngineerThread: replicaRequest.GetMapOp().arg2 = " << replicaRequest.GetMapOp().arg2 << std::endl;
			// process request

			if (replicaRequest.GetLastIndex() > committed_index)
			{
				cr_lock.lock();
				smr_lock.lock();
				smr_log.push_back(replicaRequest.GetMapOp());
                WriteToLogFile(replicaRequest.GetMapOp());
				// primary_last_index = replicaRequest.GetLastIndex();
				int primary_committed_index = replicaRequest.GetCommittedIndex();
				for (int i = committed_index + 1; i <= primary_committed_index; i++)
				{
					MapOp op = smr_log[i];
					customer_record[op.arg1] = op.arg2;
					// std::cout << "Applied map op to customer record"
					// 		  << "op.arg1 " << op.arg1 << "op.arg2 " << op.arg2 << std::endl;
					// std::cout << "EngineerThread: customer_record = " << customer_record[op.arg1] << std::endl;
				}
				last_index = replicaRequest.GetLastIndex();
				committed_index = primary_committed_index;
				cr_lock.unlock();
				smr_lock.unlock();
			}
			// std::cout << "EngineerThread: last_index = " << last_index << std::endl;
			// std::cout << "EngineerThread: committed_index = " << committed_index << std::endl;

			ReplicaResponse response;
			response.SetStatus(1);
			stub.SendReplicaResponse(response);
			continue;
		}
//		std::cout << "EngineerThread: before processing" << std::endl;
		customerRequest = stub.ReceiveRequest();
		if (!customerRequest.IsValid())
		{
//			std::cout << "Connection broken engineer" << std::endl;
			break;
		}
		request_type = customerRequest.GetLaptopType();
//		std::cout << "EngineerThread: processing request_type = " << request_type << std::endl;
		switch (request_type)
		{
		case 0: // not used in this assignment
			laptop = CreateRegularLaptop(customerRequest, engineer_id);
			stub.SendLaptop(laptop);
			break;
		case 1:
			laptop = CreateCustomLaptop(customerRequest, engineer_id);
			stub.SendLaptop(laptop);
			break;
		case 2: // read for one customer id
			cr_lock.lock();
            // simulated read request processing time
            std::this_thread::sleep_for(std::chrono::milliseconds (10));
			if (customer_record.find(customerRequest.GetCustomerId()) != customer_record.end())
			{
				int last_ind = customer_record[customerRequest.GetCustomerId()];
				record.SetRecord(customerRequest.GetCustomerId(), last_ind);
			}
			else
			{
				record.SetRecord(customerRequest.GetCustomerId(), -1);
			}
			stub.ReturnRecord(record);
			// record.Print();
			cr_lock.unlock();
			break;
		case 4:
//			std::cout << "Special customerRequest received setting back up node to true" << std::endl;
			{
				is_backup_node = true;
				primary_id = customerRequest.GetCustomerId();
				stub.SendLaptop(laptop);
			}
			break;
		case 5:
//			std::cout << "EngineerThread: replica recover request" << std::endl;
			laptop.SetInfo(customerRequest.GetCustomerId(), last_index, primary_id, 0, 0);
			stub.SendLaptop(laptop);
			break;
		case 6:
		{
			int wanted_index;
			MapOp op;
			wanted_index = customerRequest.GetOrderNumber();
			smr_lock.lock();
			op = smr_log[wanted_index];
			record.SetRecord(op.arg1, op.arg2);
			stub.ReturnRecord(record);
			smr_lock.unlock();
		}
		break;
		default:
			std::cout << "Undefined laptop type: "
					  << request_type << std::endl;
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
		// std::cout << "special engineer thread smr_lock locked" << std::endl;
		MapOp logOp = {1, req->laptop.GetCustomerId(), req->laptop.GetOrderNumber()};
		smr_log.push_back(logOp);
        WriteToLogFile(logOp);
		// std::cout << "smr_log size: " << smr_log.size() << std::endl;
		last_index += 1;
		if (primary_id != factory_id)
		{
			primary_id = factory_id;
			while (committed_index < last_index)
			{
				MapOp op = smr_log[committed_index + 1];
				customer_record[op.arg1] = op.arg2;
				committed_index++;
			}
			// MakeReplicaConnections();
			// replicas_connections_made = true;
		}
		MakeReplicaConnections();
		// std::cout << "finished creating replicas" << std::endl;
		ReplicaRequest request;
		request.SetRequest(factory_id, committed_index, last_index, {1, req->laptop.GetCustomerId(), req->laptop.GetOrderNumber()});
		for (auto &replica : replica_stubs)
		{
			// std::cout << "sending replica request to replica" << std::endl;
			// ReplicaResponse response = replica->SendReplicaRequest(request);
			replica->SendReplicaRequest(request);
			// std::cout << "recieved confirmation from replica " << response.GetStatus() << std::endl;
		}
		customer_record[req->laptop.GetCustomerId()] = req->laptop.GetOrderNumber();

		committed_index = smr_log.size() - 1;
		cr_lock.unlock();
		smr_lock.unlock();
		// std::this_thread::sleep_for(std::chrono::microseconds(100));
		req->laptop.SetExpertId(id);
		req->prom.set_value(req->laptop);
	}
}

void LaptopFactory::MakeReplicaConnections()
{
	// std::cout << "Making replica connections" << std::endl;
	replica_stubs.clear();
	for (auto &replica : replicas)
	{
		std::unique_ptr<ServerClientStub> stub(new ServerClientStub());
		if (stub->Init(replica.first, replica.second) == 0)
		{
			// std::cout << "Failed to connect to " << replica.first << ":" << replica.second << std::endl;
			continue;
		}
		// std::cout << "Made connection to " << replica.first << ":" << replica.second << std::endl;
		CustomerRequest customerRequest;
		customerRequest.SetCustomerRequest(factory_id, 0, 4);
		stub->OrderLaptop(customerRequest);
		// std::cout << "Registration Order sent to replica" << std::endl;
		replica_stubs.emplace_back(std::move(stub)); // Move the unique_ptr into the vector
	}
	// }
}

LaptopFactory::LaptopFactory()
{
	last_index = -1;
	committed_index = -1;
	primary_id = -1;
	factory_id = -1;
}

void LaptopFactory::SetFactoryId(int id)
{
	factory_id = id;
}

void LaptopFactory::AddReplica(int id, std::string ip, int port)
{
	std::pair<std::string, int> replica = {ip, port};
	replicas.push_back(replica);
	replica_id_to_ip_port[id] = replica;
}

void LaptopFactory::RecoverReplica()
{
	ServerClientStub serverClientStub;
	for (auto &replica : replicas)
	{
		// std::cout << "In RecoverReplica: Trying to connect to " << replica.first << ":" << replica.second << std::endl;
		if (serverClientStub.Init(replica.first, replica.second) != 0)
		{
			CustomerRequest customerRequest;
			CustomerRecord customerRecord;
			customerRequest.SetCustomerRequest(factory_id, last_index, 5);
			LaptopInfo otherstatus = serverClientStub.OrderLaptop(customerRequest);
			int otherserverLastIndex = otherstatus.GetOrderNumber();
			primary_id = otherstatus.GetCustomerId();
			std::cout << "Recovering data : otherserverLastIndex = " << otherserverLastIndex << " current last index " << last_index << std::endl;
			if (otherserverLastIndex > last_index)
			{
				cr_lock.lock();
				smr_lock.lock();
				for (int i = last_index + 1; i <= otherserverLastIndex; i++)
				{
					customerRequest.SetCustomerRequest(factory_id, i, 6);
					customerRecord = serverClientStub.ReadRecord(customerRequest);
					MapOp op = {1, customerRecord.GetCustomerId(), customerRecord.GetLastOrder()};
					smr_log.push_back(op);
                    WriteToLogFile(op);
					customer_record[op.arg1] = op.arg2;
					std::cout << "Recovering data Applied map op to customer record"
							  << " op.arg1 " << op.arg1 << " op.arg2 " << op.arg2 << std::endl;
				}
				cr_lock.unlock();
				smr_lock.unlock();
				last_index = otherserverLastIndex;
				committed_index = last_index;
			}
//            std::this_thread::sleep_for(std::chrono::microseconds(100));
			std::cout << "Current Replica now is up to date" << std::endl;
			break;
		}
	}
}

void LaptopFactory::RecoverFromLogFile()
{
	std::string logFilePath = std::to_string(factory_id) + ".log";
	std::ifstream logFile(logFilePath);
	std::string line;
	if (!logFile.is_open())
	{
//		std::cerr << "Failed to open log file: " << logFilePath << std::endl;
		return;
	}
	while (std::getline(logFile, line))
	{
		std::istringstream iss(line);
		std::string operationType;
		int customerId, lastOrderIndex;

		if (!(iss >> operationType >> customerId >> lastOrderIndex))
		{
//			std::cerr << "Failed to parse line: " << line << std::endl;
			continue;
		}
        MapOp op = {1, customerId, lastOrderIndex};
		smr_log.push_back(op);
        customer_record[op.arg1] = op.arg2;
//        last_index = last_index < op.arg2 ? op.arg2 : last_index;
//		std::cout << "Updated customerId " << customerId << " with lastOrderIndex " << lastOrderIndex << std::endl;
	}
    last_index = smr_log.size() - 1;
    committed_index = last_index;
	logFile.close();
//	std::cout << "Final state of customerLastOrderIndex map:" << std::endl;
//	for (const auto &entry : customer_record)
//	{
//		std::cout << "CustomerId: " << entry.first << ", LastOrderIndex: " << entry.second << std::endl;
//	}
}

void LaptopFactory::WriteToLogFile(MapOp op)
{
	// log file name same as factory id
	std::string logFilePath = std::to_string(factory_id) + ".log";
	std::ofstream logFile(logFilePath, std::ios::app);
	if (!logFile.is_open())
	{
		std::cerr << "log file not exist, will create new " << logFilePath << std::endl;
		return;
	}
//	std::cout << "Writing to log file: " << logFilePath << " 1 " << op.arg1 << " " << op.arg2 << std::endl;
	logFile << "1 " << op.arg1 << " " << op.arg2 << std::endl;
	logFile.close();
}