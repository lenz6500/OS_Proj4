#ifndef __MMU_H_
#define __MMU_H_

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>

typedef struct Variable {
    std::string name;
    int virtual_address;
    int size;
} Variable;

typedef struct Process {
    uint32_t pid;
    int p_virtual_address;//added, it tracks v_address in a process
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
    uint32_t createNewProcess(uint32_t text_size, uint32_t data_size);
};

#endif // __MMU_H_