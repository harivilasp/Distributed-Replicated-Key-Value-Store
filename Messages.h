#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include <string>

class CustomerRequest
{
private:
	int customer_id;
	int order_number;
	int request_type;

public:
	CustomerRequest();
	void operator=(const CustomerRequest &order)
	{
		customer_id = order.customer_id;
		order_number = order.order_number;
		request_type = order.request_type;
	}
	void SetCustomerRequest(int cid, int order_num, int type);
	int GetCustomerId();
	int GetOrderNumber();
	int GetLaptopType();

	int Size();

	void Marshal(char *buffer);
	void Unmarshal(char *buffer);

	bool IsValid();

	void Print();
};

class LaptopInfo
{
private:
	int customer_id;
	int order_number;
	int request_type;
	int engineer_id;
	int admin_id;

public:
	LaptopInfo();
	void operator=(const LaptopInfo &info)
	{
		customer_id = info.customer_id;
		order_number = info.order_number;
		request_type = info.request_type;
		engineer_id = info.engineer_id;
		admin_id = info.admin_id;
	}
	void SetInfo(int cid, int order_num, int type, int engid, int expid);
	void CopyOrder(CustomerRequest order);
	void SetEngineerId(int id);
	void SetExpertId(int id);

	int GetCustomerId();
	int GetOrderNumber();
	int GetLaptopType();
	int GetEngineerId();
	int GetExpertId();

	int Size();

	void Marshal(char *buffer);
	void Unmarshal(char *buffer);

	bool IsValid();

	void Print();
};

class Record
{
private:
	int customer_id;
	int last_order;

public:
	Record();
	void operator=(const Record &rec)
	{
		customer_id = rec.customer_id;
		last_order = rec.last_order;
	}
	void SetRecord(int cid, int last_order);
	int GetCustomerId();
	int GetLastOrder();

	int Size();

	void Marshal(char *buffer);
	void Unmarshal(char *buffer);

	void Print();
};

struct MapOp
{
	int opcode; // operation code : 1 - update value
	int arg1;	// customer_id to apply the operation
	int arg2;	// parameter for the operation
};

class ReplicaRequest
{
private:
	int factory_id;
	int committed_index;
	int last_index;
	MapOp op;

public:
	ReplicaRequest();
	void SetRequest(int fid, int cindex, int lindex, MapOp op);
	int GetFactoryId();
	int GetCommittedIndex();
	int GetLastIndex();
	MapOp GetMapOp();
	int Size();
	void Marshal(char *buffer);
	void Unmarshal(char *buffer);
	void Print();
	bool IsValid();
};

class ReplicaResponse
{
private:
	int status;

public:
	ReplicaResponse();
	void SetStatus(int stat);
	bool GetStatus();
	int Size();
	void Marshal(char *buffer);
	void Unmarshal(char *buffer);
	void Print();
};

#endif // #ifndef __MESSAGES_H__
