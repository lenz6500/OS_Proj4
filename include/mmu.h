#ifndef __MMU_H_
#define __MMU_H_

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstring>
#include "pagetable.h"
typedef struct Variable {
    std::string name;
    int virtual_address;
    int size;
    std::string data_type;
    std::vector<int> page_numbers;
} Variable;


typedef struct Process {
    uint32_t pid;
    int p_virtual_address;
	int _available_space;
    std::vector<Variable*> variables;
    int _next_page;
} Process;

class Mmu {
private:
    uint32_t _next_pid;
    int _max_size;
	int _total_allocated;
    std::vector<Process*> _processes;

public:
    Mmu(int memory_size);
    ~Mmu();

    uint32_t createProcess();
    void print();
    void printData(int pid, std::string& varName, PageTable *pageTable, uint8_t *memory);
    void printProcesses();
    uint32_t createNewProcess(uint32_t text_size, uint32_t data_size, PageTable *pageTable);
    int allocate( uint32_t pid, const std::string& var_name, const std::string& data_type, int num_element, PageTable *pageTable);
    int set(uint8_t *memory, uint32_t pid, std::string& var_name, uint32_t offset, std::vector<std::string> values, PageTable *pageTable);
    int free(uint32_t pid, std::string& var_name, PageTable *pageTable, uint8_t *memory);
    int terminate(uint32_t pid, PageTable *pageTable);
    int findProcess(uint32_t pid);
    int findFreeVar(int pid_index);
    int findVariableIndex(std::string& varName, int index);
    int findVariableAddr(std::string& varName, int index);
    int getDataSize(std::string& data_type);
    bool isShared(uint32_t pid, int page_number, std::string& var_name);

};

#endif // __MMU_H_
