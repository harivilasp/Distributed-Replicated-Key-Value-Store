#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "ServerSocket.h"
#include "LoadThread.h"

int main(int argc, char *argv[])
{
	int port;
	int engineer_cnt = 0;
	int num_experts = 1;
	int num_replicas = 1;
    int cache_capacity = 0;
	ServerSocket socket;
	LoadFactory factory;
	std::unique_ptr<ServerSocket> new_socket;
	std::vector<std::thread> thread_vector;

	if (argc < 3)
	{
		std::cout << "not enough arguments" << std::endl;
		std::cout << argv[0] << " [port #] [# experts] " << std::endl;
		// return 0;
		port = 12347;
		num_experts = 1;
        factory.cache_capacity = cache_capacity;
	}
	else
	{
		port = atoi(argv[1]);
        factory.cache_capacity = atoi(argv[2]);
		num_replicas = atoi(argv[3]);
		int startindex = 4;
		for (int k = 0; k < num_replicas; k++)
		{
			factory.AddReplica(atoi(argv[startindex]), argv[startindex + 1], atoi(argv[startindex + 2]));
			startindex += 3;
		}
	}
	for (int i = 0; i < num_experts; i++)
	{
		std::thread expert_thread(&LoadFactory::ExpertThread,
								  &factory, engineer_cnt++);
		thread_vector.push_back(std::move(expert_thread));
	}

	if (!socket.Init(port))
	{
		std::cout << "Socket initialization failed" << std::endl;
		return 0;
	}

	while ((new_socket = socket.Accept()))
	{
		std::thread engineer_thread(&LoadFactory::EngineerThread,
									&factory, std::move(new_socket),
									engineer_cnt++);
		thread_vector.push_back(std::move(engineer_thread));
	}
	return 0;
	// }
}
