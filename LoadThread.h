#ifndef __LOADTHREAD_H__
#define __LOADTHREAD_H__

#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <map>
#include <list>
#include <unordered_map>
#include <cassert>

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
	std::unordered_map<int, int> customer_record_cache;
	std::mutex smr_lock;
	std::mutex cr_lock;
	std::vector<std::pair<std::string, int>> replicas;
	std::map<int, std::pair<std::string, int>> replica_id_to_ip_port;
	std::vector<std::unique_ptr<ServerClientStub>> replica_stubs;
	int round_robin_counter = 0;
	std::mutex round_robin_lock;
    int primary_server_index = 0;
    std::mutex primary_server_lock;

    std::list<std::pair<int, int>> cache_list;
    std::unordered_map<int, std::list<std::pair<int, int>>::iterator> lru_map;
public:
	void EngineerThread(std::unique_ptr<ServerSocket> socket, int id);
	void ExpertThread(int id);
	LoadFactory();
	void AddReplica(int id, std::string ip, int port);
    int cache_capacity = 0;

private:
	std::unique_ptr<ServerClientStub> connect_server(int id);
    std::unique_ptr<ServerClientStub> connect_round_robin_server();
    void get_from_cache(int customer_id, int& last_order, bool& found);
    void set_in_cache(int customer_id, int last_order);
    void remove_from_cache(int customer_id);
};

#endif // end of #ifndef __SERVERTHREAD_H__
