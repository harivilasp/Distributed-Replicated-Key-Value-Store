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

struct ExpertRequest
{
	LaptopInfo laptop;
	std::promise<LaptopInfo> prom;
};

struct MapOp
{
	int opcode; // operation code : 1 - update value
	int arg1;	// customer_id to apply the operation
	int arg2;	// parameter for the operation
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

	LaptopInfo CreateRegularLaptop(LaptopOrder order, int engineer_id);
	LaptopInfo CreateCustomLaptop(LaptopOrder order, int engineer_id);

public:
	void EngineerThread(std::unique_ptr<ServerSocket> socket, int id);
	void ExpertThread(int id);
};

#endif // end of #ifndef __SERVERTHREAD_H__
