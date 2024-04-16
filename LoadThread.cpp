#include <iostream>
#include <memory>
#include <utility>

#include "LoadThread.h"
#include "ServerStub.h"
#include "LoadServerStub.h"
#include "LoadServerClientStub.h"

void LoadFactory::
	EngineerThread(std::unique_ptr<ServerSocket> socket, int id)
{
	int request_type;
	CustomerRequest customerRequest;
	LaptopInfo laptop;

	ServerStub stub;
	std::unique_ptr<ServerClientStub> client_stub;
//	client_stub = connect_server(id);
	CustomerRecord record;
	stub.Init(std::move(socket));
	while (true)
	{
//		std::cout << "LoadThread server:: EngineerThread while start " << id << std::endl;
		customerRequest = stub.ReceiveRequest();
//		std::cout << "LoadThread server:: EngineerThread request recieved " << id << std::endl;
		if (!customerRequest.IsValid())
		{
			// std::cout << "Connection broken engineer" << std::endl;
			break;
		}
		request_type = customerRequest.GetLaptopType();
		switch (request_type)
		{
		case 1: {
            client_stub = connect_server(primary_server_index);
            const int total_servers = replicas.size();
            int attempts = 0;
            while (!client_stub && attempts < total_servers) {
                std::lock_guard<std::mutex> guard(primary_server_lock);
                primary_server_index = (primary_server_index + 1) % total_servers;
                client_stub = connect_server(primary_server_index);
                attempts++;
            }
//            std::cout << "LoadThread server:: requesting laptop " << id << std::endl;
            // remove from the cache for this customer ID
            remove_from_cache(customerRequest.GetCustomerId());
            laptop = client_stub->OrderLaptop(customerRequest);
//            std::cout << "LoadThread server:: laptop received " << id << std::endl;
            // set in cache and send back to client
            stub.SendLaptop(laptop);
            break;
        }
        case 2: // read for one customer id
        {
            client_stub = connect_round_robin_server();
//            std::cout << "LoadThread server:: read request received " << id << std::endl;
            cr_lock.lock();
            int last_order;
            bool found;
            get_from_cache(customerRequest.GetCustomerId(), last_order, found);
            if (found) {
//                std::cout << "LoadThread server:: Read record from cache " << id << std::endl;
                record.SetRecord(customerRequest.GetCustomerId(), last_order);
            } else {
                CustomerRecord temp = client_stub->ReadRecord(customerRequest);
//                std::cout << "LoadThread server:: Read record from server " << id << std::endl;
//                temp.Print();
                // also set in cache
                if (temp.GetLastOrder() != -1)
                {
                    set_in_cache(customerRequest.GetCustomerId(), temp.GetLastOrder());
                }
                record.SetRecord(customerRequest.GetCustomerId(), temp.GetLastOrder());
            }
//            std::cout << "LoadThread server:: read request returning " << id << std::endl;
            stub.ReturnRecord(record);
            cr_lock.unlock();
            break;
        }
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

}

void LoadFactory::AddReplica(int id, std::string ip, int port)
{
	std::pair<std::string, int> replica = {ip, port};
	replicas.push_back(replica);
	replica_id_to_ip_port[id] = replica;
}

std::unique_ptr<ServerClientStub> LoadFactory::connect_server(int ind) {
    std::unique_ptr<ServerClientStub> stub = std::unique_ptr<ServerClientStub>(new ServerClientStub());
    if (stub->Init(replicas[ind].first, replicas[ind].second))
    {
//        std::cout << "LoadThread:: connected to server " << ind
//        << " : " << replicas[ind].first
//        <<" : "<< replicas[ind].second << std::endl;
    } else {
        return nullptr;
    }
    return stub;
}

std::unique_ptr<ServerClientStub> LoadFactory::connect_round_robin_server() {
    std::lock_guard<std::mutex> guard(round_robin_lock);
    std::unique_ptr<ServerClientStub> stub = nullptr;
    const int total_servers = replicas.size();
    int attempts = 0;
    while (!stub && attempts < total_servers) {
        int serverIndex = round_robin_counter % total_servers;
        stub = connect_server(serverIndex);
        round_robin_counter++;
        attempts++;
    }
    if (!stub) {
        std::cout << "LoadThread:: Could not connect to any server" << std::endl;
    }
    return stub;
}

void LoadFactory::get_from_cache(int customer_id, int& last_order, bool& found) {
//    std::cout << "LoadThread:: Getting from cache " << customer_id << std::endl;
//    for (auto& item : cache_list) {
//        std::cout << "LoadThread:: Cache item " << item.first << " : " << item.second << std::endl;
//    }
    auto mapIt = lru_map.find(customer_id);
    if (mapIt != lru_map.end()) {
        found = true;
        last_order = mapIt->second->second;
        cache_list.erase(mapIt->second); // Remove the existing occurrence
        cache_list.push_front({customer_id, last_order}); // Insert it at the back
        lru_map.erase(mapIt);
        lru_map[customer_id] = cache_list.begin();
    } else {
//        std:: cout << "LoadThread:: Not found in cache " << customer_id << std::endl;
        found = false;
    }
}

void LoadFactory::set_in_cache(int customer_id, int last_order) {
//    std::cout << "LoadThread:: Setting in cache " << customer_id << " : " << last_order << std::endl;
    auto mapIt = lru_map.find(customer_id);
    // Check if the item already exists
    if (mapIt != lru_map.end()) {
        lru_map.erase(mapIt);
        cache_list.erase(mapIt->second);
    }
    cache_list.push_front({customer_id, last_order});
    lru_map[customer_id] = cache_list.begin();
//    std::cout << "LoadThread:: Cache size " << lru_map.size() << std::endl;
    if (lru_map.size() > static_cast<std::size_t>(cache_capacity)) {
        // If the cache is full, remove the least recently used item
        int lru = cache_list.back().first;
        lru_map.erase(lru);
        cache_list.pop_back();
    }
}

void LoadFactory::remove_from_cache(int customer_id) {
//    std::cout << "LoadThread:: Removing from cache " << customer_id << std::endl;
    std::lock_guard<std::mutex> lock(cr_lock); // Use the cache mutex to ensure thread safety
    auto it = lru_map.find(customer_id);
    if (it != lru_map.end()) {
        cache_list.erase(it->second);
        // If the customer_id is found in the cache, remove it
        lru_map.erase(it);
    }
}