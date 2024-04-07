#ifndef __LOADTHREAD_H__
#define __LOADTHREAD_H__

#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <map>

#include "Messages.h"
#include "ServerSocket.h"
#include "ServerClientStub.h"

struct LoadRequest
{
	LaptopInfo laptop;
	std::promise<LaptopInfo> prom;
};

class LoadFactory
{
private:
	std::queue<std::unique_ptr<LoadRequest>> erq;
	std::mutex erq_lock;
	std::condition_variable erq_cv;
	std::vector<MapOp> smr_log;
	std::map<int, int> customer_record_cache;
	std::mutex smr_lock;
	std::mutex cr_lock;
	std::vector<std::pair<std::string, int>> replicas;
	std::map<int, std::pair<std::string, int>> replica_id_to_ip_port;
	std::vector<std::unique_ptr<ServerClientStub>> replica_stubs;
	int round_robin_counter = 0;
	std::mutex round_robin_lock;
    int primary_server_index = 0;
    std::mutex primary_server_lock;

	bool replicas_connections_made = false;
	int last_index;		 // the last index of the smr_log that has data
	int committed_index; // the last index of the smr_log where the
						 // MapOp of the log entry is committed and
						 // applied to the customer_record_cache
	int primary_id;		 // the production factory id ( server id ).
						 // initially set to -1.
	int factory_id;		 // the id of the factory . This is assigned via
						 // the command line arguments .

	LaptopInfo CreateRegularLaptop(CustomerRequest order, int engineer_id);
	LaptopInfo CreateCustomLaptop(CustomerRequest order, int engineer_id);

public:
	void EngineerThread(std::unique_ptr<ServerSocket> socket, int id);
	void ExpertThread(int id);
	LoadFactory();
	void MakeReplicaConnections();
	void AddReplica(int id, std::string ip, int port);

private:
	std::unique_ptr<ServerClientStub> connect_server(int id);
    std::unique_ptr<ServerClientStub> connect_round_robin_server();
};

#endif // end of #ifndef __SERVERTHREAD_H__
