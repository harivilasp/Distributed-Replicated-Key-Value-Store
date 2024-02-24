#include <array>
#include <iostream>
#include <iomanip>
#include <thread>
#include <vector>

#include "ClientSocket.h"
#include "ClientThread.h"
#include "ClientTimer.h"

int main(int argc, char *argv[])
{
	std::string ip;
	int port;
	int customer_id;
	int num_orders;
	int request_type;
	ClientTimer timer;

	std::vector<std::shared_ptr<ClientThreadClass>> client_vector;
	std::vector<std::thread> thread_vector;

	if (argc < 6)
	{
		std::cout << "not enough arguments" << std::endl;
		std::cout << argv[0] << "[ip] [port #] [# customers] ";
		std::cout << "[# orders] [laptop type 0 or 1]" << std::endl;
		// return 0;
		ip = "127.0.0.1";
		port = 12347;
		customer_id = 1;
		num_orders = 1;
		request_type = 1;
	}
	else
	{
		ip = argv[1];
		port = atoi(argv[2]);
		customer_id = atoi(argv[3]);
		num_orders = atoi(argv[4]);
		request_type = atoi(argv[5]);
	}

	timer.Start();
	// for (int i = 0; i < customer_id; i++)
	// {
	// 	auto client_cls = std::shared_ptr<ClientThreadClass>(new ClientThreadClass());
	// 	std::thread client_thread(&ClientThreadClass::ThreadBody, client_cls,
	// 							  ip, port, i, num_orders, request_type);

	// 	client_vector.push_back(std::move(client_cls));
	// 	thread_vector.push_back(std::move(client_thread));
	// }
	auto client_cls = std::shared_ptr<ClientThreadClass>(new ClientThreadClass());
	std::thread client_thread(&ClientThreadClass::ThreadBody, client_cls,
							  ip, port, customer_id, num_orders, request_type);

	client_vector.push_back(std::move(client_cls));
	thread_vector.push_back(std::move(client_thread));
	for (auto &th : thread_vector)
	{
		th.join();
	}
	timer.End();

	for (auto &cls : client_vector)
	{
		timer.Merge(cls->GetTimer());
	}
	timer.PrintStats();

	return 1;
}
