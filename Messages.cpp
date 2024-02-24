#include <cstring>
#include <iostream>

#include <arpa/inet.h>
#include "Messages.h"

CustomerRequest::CustomerRequest()
{
	customer_id = -1;
	order_number = -1;
	request_type = -1;
}

void CustomerRequest::SetCustomerRequest(int id, int number, int type)
{
	customer_id = id;
	order_number = number;
	request_type = type;
}

int CustomerRequest::GetCustomerId() { return customer_id; }
int CustomerRequest::GetOrderNumber() { return order_number; }
int CustomerRequest::GetLaptopType() { return request_type; }

int CustomerRequest::Size()
{
	return sizeof(customer_id) + sizeof(order_number) + sizeof(request_type);
}

void CustomerRequest::Marshal(char *buffer)
{
	int net_customer_id = htonl(customer_id);
	int net_order_number = htonl(order_number);
	int net_laptop_type = htonl(request_type);
	int offset = 0;
	memcpy(buffer + offset, &net_customer_id, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(buffer + offset, &net_order_number, sizeof(net_order_number));
	offset += sizeof(net_order_number);
	memcpy(buffer + offset, &net_laptop_type, sizeof(net_laptop_type));
}

void CustomerRequest::Unmarshal(char *buffer)
{
	int net_customer_id;
	int net_order_number;
	int net_laptop_type;
	int offset = 0;
	memcpy(&net_customer_id, buffer + offset, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(&net_order_number, buffer + offset, sizeof(net_order_number));
	offset += sizeof(net_order_number);
	memcpy(&net_laptop_type, buffer + offset, sizeof(net_laptop_type));

	customer_id = ntohl(net_customer_id);
	order_number = ntohl(net_order_number);
	request_type = ntohl(net_laptop_type);
}

bool CustomerRequest::IsValid()
{
	return (customer_id != -1);
}

void CustomerRequest::Print()
{
	std::cout << "id " << customer_id << " ";
	std::cout << "num " << order_number << " ";
	std::cout << "type " << request_type << std::endl;
}

LaptopInfo::LaptopInfo()
{
	customer_id = -1;
	order_number = -1;
	request_type = -1;
	engineer_id = -1;
	admin_id = -1;
}

void LaptopInfo::SetInfo(int id, int number, int type, int engid, int expid)
{
	customer_id = id;
	order_number = number;
	request_type = type;
	engineer_id = engid;
	admin_id = expid;
}

void LaptopInfo::CopyOrder(CustomerRequest order)
{
	customer_id = order.GetCustomerId();
	order_number = order.GetOrderNumber();
	request_type = order.GetLaptopType();
}
void LaptopInfo::SetEngineerId(int id) { engineer_id = id; }
void LaptopInfo::SetExpertId(int id) { admin_id = id; }

int LaptopInfo::GetCustomerId() { return customer_id; }
int LaptopInfo::GetOrderNumber() { return order_number; }
int LaptopInfo::GetLaptopType() { return request_type; }
int LaptopInfo::GetEngineerId() { return engineer_id; }
int LaptopInfo::GetExpertId() { return admin_id; }

int LaptopInfo::Size()
{
	return sizeof(customer_id) + sizeof(order_number) + sizeof(request_type) + sizeof(engineer_id) + sizeof(admin_id);
}

void LaptopInfo::Marshal(char *buffer)
{
	int net_customer_id = htonl(customer_id);
	int net_order_number = htonl(order_number);
	int net_laptop_type = htonl(request_type);
	int net_engineer_id = htonl(engineer_id);
	int net_expert_id = htonl(admin_id);
	int offset = 0;

	memcpy(buffer + offset, &net_customer_id, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(buffer + offset, &net_order_number, sizeof(net_order_number));
	offset += sizeof(net_order_number);
	memcpy(buffer + offset, &net_laptop_type, sizeof(net_laptop_type));
	offset += sizeof(net_laptop_type);
	memcpy(buffer + offset, &net_engineer_id, sizeof(net_engineer_id));
	offset += sizeof(net_engineer_id);
	memcpy(buffer + offset, &net_expert_id, sizeof(net_expert_id));
}

void LaptopInfo::Unmarshal(char *buffer)
{
	int net_customer_id;
	int net_order_number;
	int net_laptop_type;
	int net_engineer_id;
	int net_expert_id;
	int offset = 0;

	memcpy(&net_customer_id, buffer + offset, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(&net_order_number, buffer + offset, sizeof(net_order_number));
	offset += sizeof(net_order_number);
	memcpy(&net_laptop_type, buffer + offset, sizeof(net_laptop_type));
	offset += sizeof(net_laptop_type);
	memcpy(&net_engineer_id, buffer + offset, sizeof(net_engineer_id));
	offset += sizeof(net_engineer_id);
	memcpy(&net_expert_id, buffer + offset, sizeof(net_expert_id));

	customer_id = ntohl(net_customer_id);
	order_number = ntohl(net_order_number);
	request_type = ntohl(net_laptop_type);
	engineer_id = ntohl(net_engineer_id);
	admin_id = ntohl(net_expert_id);
}

bool LaptopInfo::IsValid()
{
	return (customer_id != -1);
}

void LaptopInfo::Print()
{
	std::cout << "id " << customer_id << " ";
	std::cout << "num " << order_number << " ";
	std::cout << "type " << request_type << " ";
	std::cout << "engid " << engineer_id << " ";
	std::cout << "expid " << admin_id << std::endl;
}

CustomerRecord::CustomerRecord()
{
	customer_id = -1;
	last_order = -1;
}

void CustomerRecord::SetRecord(int cid, int last_order_id)
{
	customer_id = cid;
	last_order = last_order_id;
}
int CustomerRecord::GetCustomerId() { return customer_id; }
int CustomerRecord::GetLastOrder() { return last_order; }

int CustomerRecord::Size() { return sizeof(customer_id) + sizeof(last_order); }

void CustomerRecord::Marshal(char *buffer)
{
	int net_customer_id = htonl(customer_id);
	int net_last_order = htonl(last_order);
	int offset = 0;

	memcpy(buffer + offset, &net_customer_id, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(buffer + offset, &net_last_order, sizeof(net_last_order));
}
void CustomerRecord::Unmarshal(char *buffer)
{
	int net_customer_id;
	int net_last_order;
	int offset = 0;

	memcpy(&net_customer_id, buffer + offset, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(&net_last_order, buffer + offset, sizeof(net_last_order));

	customer_id = ntohl(net_customer_id);
	last_order = ntohl(net_last_order);
}

void CustomerRecord::Print()
{
	std::cout << "id " << customer_id << " ";
	std::cout << "last " << last_order << std::endl;
}

ReplicaRequest::ReplicaRequest()
{
	factory_id = -1;
	committed_index = -1;
	last_index = -1;
	op = {0, 0, 0};
}

void ReplicaRequest::SetRequest(int fid, int cindex, int lindex, MapOp op)
{
	factory_id = fid;
	committed_index = cindex;
	last_index = lindex;
	this->op = op;
}

int ReplicaRequest::GetFactoryId() { return factory_id; }
int ReplicaRequest::GetCommittedIndex() { return committed_index; }
int ReplicaRequest::GetLastIndex() { return last_index; }
MapOp ReplicaRequest::GetMapOp() { return op; }
int ReplicaRequest::Size() { return sizeof(factory_id) + sizeof(committed_index) + sizeof(last_index) + 3 * sizeof(op.arg1); }
void ReplicaRequest::Marshal(char *buffer)
{

	int net_factory_id = htonl(factory_id);
	int net_committed_index = htonl(committed_index);
	int net_last_index = htonl(last_index);
	int net_opcode = htonl(op.opcode);
	int net_arg1 = htonl(op.arg1);
	int net_arg2 = htonl(op.arg2);
	int offset = 0;

	memcpy(buffer + offset, &net_factory_id, sizeof(net_factory_id));
	offset += sizeof(net_factory_id);
	memcpy(buffer + offset, &net_committed_index, sizeof(net_committed_index));
	offset += sizeof(net_committed_index);
	memcpy(buffer + offset, &net_last_index, sizeof(net_last_index));
	offset += sizeof(net_last_index);
	memcpy(buffer + offset, &net_opcode, sizeof(net_opcode));
	offset += sizeof(net_opcode);
	memcpy(buffer + offset, &net_arg1, sizeof(net_arg1));
	offset += sizeof(net_arg1);
	memcpy(buffer + offset, &net_arg2, sizeof(net_arg2));
}
void ReplicaRequest::Unmarshal(char *buffer)
{
	int net_factory_id;
	int net_committed_index;
	int net_last_index;
	int net_opcode;
	int net_arg1;
	int net_arg2;
	int offset = 0;

	memcpy(&net_factory_id, buffer + offset, sizeof(net_factory_id));
	offset += sizeof(net_factory_id);
	memcpy(&net_committed_index, buffer + offset, sizeof(net_committed_index));
	offset += sizeof(net_committed_index);
	memcpy(&net_last_index, buffer + offset, sizeof(net_last_index));
	offset += sizeof(net_last_index);
	memcpy(&net_opcode, buffer + offset, sizeof(net_opcode));
	offset += sizeof(net_opcode);
	memcpy(&net_arg1, buffer + offset, sizeof(net_arg1));
	offset += sizeof(net_arg1);
	memcpy(&net_arg2, buffer + offset, sizeof(net_arg2));

	factory_id = ntohl(net_factory_id);
	committed_index = ntohl(net_committed_index);
	last_index = ntohl(net_last_index);
	op.opcode = ntohl(net_opcode);
	op.arg1 = ntohl(net_arg1);
	op.arg2 = ntohl(net_arg2);
}
void ReplicaRequest::Print()
{
	std::cout << "factory_id " << factory_id << " ";
	std::cout << "committed_index " << committed_index << " ";
	std::cout << "last_index " << last_index << " ";
	std::cout << "opcode " << op.opcode << " ";
	std::cout << "arg1 " << op.arg1 << " ";
	std::cout << "arg2 " << op.arg2 << std::endl;
}

bool ReplicaRequest::IsValid()
{
	return (last_index != -1);
}

ReplicaResponse::ReplicaResponse() { status = 0; }
void ReplicaResponse::SetStatus(int stat) { status = stat; }
bool ReplicaResponse::GetStatus() { return status; }
int ReplicaResponse::Size() { return sizeof(status); }
void ReplicaResponse::Marshal(char *buffer)
{
	int net_status = htonl(status);
	memcpy(buffer, &net_status, sizeof(net_status));
}
void ReplicaResponse::Unmarshal(char *buffer)
{
	int net_status;
	memcpy(&net_status, buffer, sizeof(net_status));
	status = ntohl(net_status);
}
void ReplicaResponse::Print() { std::cout << "status " << status << std::endl; }