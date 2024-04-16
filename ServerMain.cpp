#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "ServerSocket.h"
#include "ServerThread.h"

int main(int argc, char *argv[])
{
	int port;
	int engineer_cnt = 0;
	int num_experts = 1;
	int id_factory;
	int num_replicas = 1;
    int is_recover_from_log = 1;
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
			factory.AddReplica(atoi(argv[startindex]), argv[startindex + 1], atoi(argv[startindex + 2]));
			startindex += 3;
		}
	}
    // ask user if they want to recover from log
    std::cout << "Do you want to recover from log? (1 for yes, 0 for no): ";
    std::cin >> is_recover_from_log;
    std::cout << " recover from log set to " << is_recover_from_log << std::endl;
	factory.SetFactoryId(id_factory);
    // log file name match with factory id
    // note time for recovery
    std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
    if (is_recover_from_log)
    {
        factory.RecoverFromLogFile();
    }
	factory.RecoverReplica();
    std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
    long time = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    std::cout << "Recovery time: " << time << " microseconds" << std::endl;
	for (int i = 0; i < num_experts; i++)
	{
		std::thread expert_thread(&LaptopFactory::ExpertThread,
								  &factory, engineer_cnt++);
		thread_vector.push_back(std::move(expert_thread));
	}

	if (!socket.Init(port))
	{
		std::cout << "Socket initialization failed: Please wait for 2 seconds and retry" << std::endl;
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
	// }
}
