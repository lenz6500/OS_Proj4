#include "mmu.h"

Mmu::Mmu(int memory_size)
{
    _next_pid = 1024;
    _max_size = memory_size;
}

Mmu::~Mmu()
{
}

uint32_t Mmu::createProcess()
{
    Process *proc = new Process();
    proc->p_virtual_address = 0;
    proc->pid = _next_pid;

    Variable *var = new Variable();
    var->name = "<FREE_SPACE>";
    var->virtual_address = 0;
    var->size = _max_size;
    proc->variables.push_back(var);

    _processes.push_back(proc);

    _next_pid++;
    return proc->pid;
}
uint32_t Mmu::createNewProcess(uint32_t text_size, uint32_t data_size)
{
    	int i, j;
	Process *proc;
	bool found = false;
    	for (i = 0; i < _processes.size() && !found; i++)
    	{
        	for (j = 0; j < _processes[i]->variables.size() && !found; j++)
        	{
            		if( _processes[i]->variables[j]->name.compare("<FREE_SPACE>")==0 )
			{
				_processes[i]->variables[j]->name = "<TEXT>";
				_processes[i]->variables[j]->virtual_address = _processes[i]->p_virtual_address;
				_processes[i]->variables[j]->size = text_size;

				_processes[i]->p_virtual_address = _processes[i]->p_virtual_address + _processes[i]->variables[j]->size;

				Variable *globals = new Variable();
				globals->name = "<GLOBALS>";
    				globals->virtual_address = _processes[i]->p_virtual_address;
    				globals->size = data_size;
				_processes[i]->variables.push_back(globals);

				_processes[i]->p_virtual_address = _processes[i]->p_virtual_address + globals->size;
				
				Variable *stack = new Variable();
				stack->name = "<STACK>";
    				stack->virtual_address = _processes[i]->p_virtual_address;
    				stack->size = 65536;
				_processes[i]->variables.push_back(stack);

				createProcess();

				found = true;
			}
        	}
    	}
	return 0;
}

void Mmu::print()
{
    int i, j;
    std::stringstream stream;
    char space = ' ';
    std::string hex;

    std::cout << " PID  | Variable Name | Virtual Addr | Size" << std::endl;
    std::cout << "------+---------------+--------------+------------" << std::endl;
    for (i = 0; i < _processes.size(); i++)
    {
        for (j = 0; j < _processes[i]->variables.size(); j++)
        {
            // TODO: print all variables (excluding <FREE_SPACE> entries)
		if( _processes[i]->variables[j]->name.compare("<FREE_SPACE>")==0 )
		{
			break;
		}
		std::stringstream stream;
		stream  << "0x"
			<< std::setfill('0')
			<< std::setw(8)
			<< std::hex
			<< _processes[i]->variables[j]->virtual_address;
		hex =stream.str();
		std::replace(hex.begin(), hex.end(), 'a', 'A');
		std::replace(hex.begin(), hex.end(), 'b', 'B');
		std::replace(hex.begin(), hex.end(), 'c', 'C');
		std::replace(hex.begin(), hex.end(), 'd', 'D');
		std::replace(hex.begin(), hex.end(), 'e', 'E');
		std::replace(hex.begin(), hex.end(), 'f', 'F');

	    std::cout << _processes[i]->pid
	              << std::string( 2, space )
	    	      << "| " 
		      << _processes[i]->variables[j]->name
	              << std::string( 14 - _processes[i]->variables[j]->name.length(), space )
		      << "|   "
		      << hex
	              << " |"
	              << std::string( 11 - std::to_string(_processes[i]->variables[j]->size).length() , space )
		      << _processes[i]->variables[j]->size <<"\n";
        }
    }
}
