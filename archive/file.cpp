#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

struct MapOp
{
    int opcode; // operation code : 1 - update value
    int arg1;	// customer_id to apply the operation
    int arg2;	// parameter for the operation
};

//void WriteToLogFile(MapOp op)
//{
//    // log file name same as factory id
//    std::string logFilePath = std::to_string(1) + ".log";
//    std::ofstream logFile(logFilePath);
//    if (!logFile.is_open())
//    {
//        std::cerr << "Failed to open log file: " << logFilePath << std::endl;
//        return;
//    }
//    std::cout << "Writing to log file: " << logFilePath << " 1 " << op.arg1 << " " << op.arg2 << std::endl;
//    logFile << "1 " << op.arg1 << " " << op.arg2 << std::endl;
//    logFile.close();
//}

void WriteToLogFile(MapOp op)
{
    // log file name same as factory id
    std::string logFilePath = std::to_string(1) + ".log";

    // Open the file in append mode
    std::ofstream logFile(logFilePath, std::ios::app);

    if (!logFile.is_open())
    {
        std::cerr << "Failed to open log file: " << logFilePath << std::endl;
        return;
    }

    std::cout << "Appending to log file: " << logFilePath << " 1 " << op.arg1 << " " << op.arg2 << std::endl;
    logFile << "1 " << op.arg1 << " " << op.arg2 << std::endl;
    logFile.close();
}


void RecoverFromLogFile()
{
    std::string logFilePath = std::to_string(1) + ".log";
    std::ifstream logFile(logFilePath);
    std::string line;
    if (!logFile.is_open())
    {
        std::cerr << "Failed to open log file: " << logFilePath << std::endl;
        return;
    }
    while (std::getline(logFile, line))
    {
        std::istringstream iss(line);
        std::string operationType;
        int customerId, lastOrderIndex;

        if (!(iss >> operationType >> customerId >> lastOrderIndex))
        {
            std::cerr << "Failed to parse line: " << line << std::endl;
            continue;
        }
//        smr_log.push_back({1, customerId, lastOrderIndex});
        std::cout << "Updated customerId " << customerId << " with lastOrderIndex " << lastOrderIndex << std::endl;
    }

    logFile.close();
    std::cout << "Final state of customerLastOrderIndex map:" << std::endl;
//    for (const auto &entry : customer_record)
//    {
//        std::cout << "CustomerId: " << entry.first << ", LastOrderIndex: " << entry.second << std::endl;
//    }
}

int main()
{
    MapOp op;
    op.opcode = 1;
    op.arg1 = 1;
    op.arg2 = 2;
    WriteToLogFile(op);
    RecoverFromLogFile();
    return 0;
}