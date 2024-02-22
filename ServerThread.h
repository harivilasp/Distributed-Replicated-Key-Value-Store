#ifndef __SERVERTHREAD_H__
#define __SERVERTHREAD_H__

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

struct ExpertRequest
{
	LaptopInfo laptop;
	std::promise<LaptopInfo> prom;
};

class LaptopFactory
{
private:
	std::queue<std::unique_ptr<ExpertRequest>> erq;
	std::mutex erq_lock;
	std::condition_variable erq_cv;
	std::vector<MapOp> smr_log;
	std::map<int, int> customer_record;
	std::mutex smr_lock;
	std::mutex cr_lock;
	std::vector<std::pair<std::string, int>> replicas;
	std::vector<std::unique_ptr<ServerClientStub>> replica_stubs;
	bool is_backup_node = false;
	bool replicas_connections_made = false;
	int last_index;		 // the last index of the smr_log that has data
	int committed_index; // the last index of the smr_log where the
						 // MapOp of the log entry is committed and
						 // applied to the customer_record
	int primary_id;		 // the production factory id ( server id ).
						 // initially set to -1.
	int factory_id;		 // the id of the factory . This is assigned via
						 // the command line arguments .

	LaptopInfo CreateRegularLaptop(LaptopOrder order, int engineer_id);
	LaptopInfo CreateCustomLaptop(LaptopOrder order, int engineer_id);

public:
	void EngineerThread(std::unique_ptr<ServerSocket> socket, int id);
	void ExpertThread(int id);
	LaptopFactory();
	void MakeReplicaConnections();
};

#endif // end of #ifndef __SERVERTHREAD_H__
