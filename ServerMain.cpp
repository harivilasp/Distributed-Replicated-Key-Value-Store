#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <map>

#include "ServerSocket.h"
#include "ServerThread.h"

int main(int argc, char *argv[])
{
	int port;
	int engineer_cnt = 0;
	int num_experts = 1;
	int id_factory;
	int num_replicas = 1;
	ServerSocket socket;
	LaptopFactory factory;
	std::unique_ptr<ServerSocket> new_socket;
	std::vector<std::thread> thread_vector;

	if (argc < 3)
	{
		std::cout << "not enough arguments" << std::endl;
		std::cout << argv[0] << " [port #] [# experts] " << std::endl;
		// return 0;
		port = 12347;
		num_experts = 1;
		id_factory = 111;
	}
	else
	{
		port = atoi(argv[1]);
		id_factory = atoi(argv[2]);
		num_replicas = atoi(argv[3]);
		int startindex = 4;
		for (int k = 0; k < num_replicas; k++)
		{
			factory.AddReplica(argv[startindex + 1], atoi(argv[startindex + 2]));
			startindex += 3;
		}
		factory.SetFactoryId(id_factory);

		for (int i = 0; i < num_experts; i++)
		{
			std::thread expert_thread(&LaptopFactory::ExpertThread,
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
			std::thread engineer_thread(&LaptopFactory::EngineerThread,
										&factory, std::move(new_socket),
										engineer_cnt++);
			thread_vector.push_back(std::move(engineer_thread));
		}
		return 0;
	}
}
