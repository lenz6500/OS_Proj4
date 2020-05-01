#ifndef __MMU_H_
#define __MMU_H_

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "pagetable.h"
typedef struct Variable {
    std::string name;
    int virtual_address;
    int size;
    std::vector<std::string> data;
} Variable;


typedef struct Process {
    uint32_t pid;
    int p_virtual_address;//added, it tracks v_address in a process
    int _max_page_size;
    std::vector<Variable*> variables;
} Process;

class Mmu {
private:
    uint32_t _next_pid;
    int _max_size;
    std::vector<Process*> _processes;

public:
    Mmu(int memory_size);
    ~Mmu();

    uint32_t createProcess();
    void print();
    void printData(int pid, std::string& varName);
    void printProcesses();
    uint32_t createNewProcess(uint32_t text_size, uint32_t data_size, PageTable *pageTable);
    int allocate( uint32_t pid, const std::string& var_name, const std::string& data_type, int num_element );
    int set(uint8_t *memory, uint32_t pid, std::string& var_name, uint32_t offset, std::vector<std::string> values, PageTable *pageTable);
    int free(uint32_t pid, std::string& var_name);
    int terminate(uint32_t pid);
    int findProcess(uint32_t pid);
    int findFreeVar(int pid_index);
    int findVariableIndex(std::string& varName, int index);
    int findVariableAddr(std::string& varName, int index);
};

#endif // __MMU_H_
