#include "mmu.h"
#include "pagetable.h"
#define STACK_SIZE 65536
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
uint32_t Mmu::createNewProcess(uint32_t text_size, uint32_t data_size, PageTable *pageTable)
{
    	int i, j, max_size, number_of_pages;
	bool found = false;
	uint32_t pid = createProcess();
	int index = findProcess(pid);//no error

	if( _processes[index]->variables[0]->name.compare("<FREE_SPACE>")==0 )
	{
		_processes[index]->variables[0]->name = "<TEXT>";
		_processes[index]->variables[0]->virtual_address = _processes[index]->p_virtual_address;
		_processes[index]->variables[0]->size = text_size;

		_processes[index]->p_virtual_address = _processes[index]->p_virtual_address + text_size;

		Variable *globals = new Variable();
		globals->name = "<GLOBALS>";
    		globals->virtual_address = _processes[index]->p_virtual_address;
    		globals->size = data_size;
		_processes[index]->variables.push_back(globals);

		_processes[index]->p_virtual_address = _processes[index]->p_virtual_address + globals->size;

		Variable *stack = new Variable();
		stack->name = "<STACK>";
    		stack->virtual_address = _processes[index]->p_virtual_address;
    		stack->size = 65536;
		_processes[index]->variables.push_back(stack);

		_processes[index]->p_virtual_address = _processes[index]->p_virtual_address + stack->size;

    		Variable *var = new Variable();
    		var->name = "<FREE_SPACE>";
    		var->virtual_address = 0;
    		var->size = _max_size - _processes[index]->p_virtual_address;
		_processes[index]->variables.push_back(var);

		//Create page table
		number_of_pages = (int)(text_size+data_size+STACK_SIZE)/pageTable->getPageSize();

		for(i=0; i<=number_of_pages; i++)
		{
			pageTable->addEntry(pid, i);
		}
	}
	return pid;
}
int Mmu::allocate( uint32_t pid, const std::string& var_name, const std::string& data_type, int num_element )
{
	int index = findProcess(pid);
	if( index == -1 )
	{ 
		return -1; //error
	}
	int var_index = findFreeVar(index);
	if( var_index == -1 )
	{
		return -1; //error
	}

	//Need to check if FREE SPACE has enough space for the input size.

	_processes[index]->variables[var_index]->name = var_name;
	_processes[index]->variables[var_index]->virtual_address = _processes[index]->p_virtual_address;

	int virtual_addr = 0;

	//set size
	if( data_type.compare("char") == 0 )
	{ 
		_processes[index]->variables[var_index]->size = num_element;
	}
	else if( data_type.compare("short") == 0 )
	{ 
		_processes[index]->variables[var_index]->size = 2*num_element;
	}
	else if( data_type.compare("int") == 0 || data_type.compare("float") == 0 )
	{ 
		_processes[index]->variables[var_index]->size = 4*num_element;
	}
	else if( data_type.compare("long") == 0 || data_type.compare("double") == 0 )
	{ 
		_processes[index]->variables[var_index]->size = 8*num_element;
	}
    	
	virtual_addr = _processes[index]->p_virtual_address;
	_processes[index]->p_virtual_address +=  _processes[index]->variables[var_index]->size;

	//Add and Track FREE SPACE
	Variable *var = new Variable();
    	var->name = "<FREE_SPACE>";
    	var->virtual_address = _processes[index]->p_virtual_address;
    	var->size = _max_size - _processes[index]->p_virtual_address;
	_processes[index]->variables.push_back(var);

	return virtual_addr;
}
int set(uint32_t pid, const std::string& var_name, const std::vector<std::string> &values, PageTable *pageTable)
{
	return 0;
}
int free(uint32_t pid, const std::string& var_name, PageTable *pageTable)
{
	return 0;
}
int Mmu::terminate(uint32_t pid)
{
	int pid_index = findProcess(pid);
	if( pid_index == -1 )
	{ 
		return -1; //error
	}

	_processes[pid_index]->variables.clear();
	//not sure
	_max_size = _max_size + _processes[pid_index]->p_virtual_address;
	delete _processes[pid_index];
	
	return 0;
}
int Mmu::findProcess(uint32_t pid)
{
    	for (int i = 0; i < _processes.size(); i++)
    	{
		if( _processes[i]->pid == pid )
		{ 
			return i;
		}
	}
	return -1;
}
int Mmu::findFreeVar(int pid_index)
{
	for (int j = 0; j < _processes[pid_index]->variables.size(); j++)
	{
		if( _processes[pid_index]->variables[j]->name.compare("<FREE_SPACE>")==0 )
		{
			return j;
		}
	}
	return -1;
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
		hex = stream.str();
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
		      << _processes[i]->variables[j]->size << std:: endl;
        }
    }
}
void Mmu::printProcesses()
{
    for (int i = 0; i < _processes.size(); i++)
    {
	std::cout << _processes[i]->pid << std:: endl;
    }
}
