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
    proc->_next_page = 0;

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
	//May need to check enough space to create a process.
    	int i, j, number_of_pages;
	uint32_t pid = createProcess();
	int index = findProcess(pid);//no error

	if( _processes[index]->variables[0]->name.compare("<FREE_SPACE>")==0 )
	{
		_processes[index]->variables[0]->name = "<TEXT>";
		_processes[index]->variables[0]->virtual_address = _processes[index]->p_virtual_address;
		_processes[index]->variables[0]->size = text_size;
		
		_processes[index]->p_virtual_address +=  text_size;
		number_of_pages = _processes[index]->p_virtual_address / pageTable->getPageSize();
		for(i=_processes[index]->_next_page; i<=number_of_pages; i++)
		{
			pageTable->addEntry(pid, i);
			_processes[index]->_next_page++;
			_processes[index]->variables[0]->page_numbers.push_back(i);
		}
		
		Variable *globals = new Variable();
		globals->name = "<GLOBALS>";
    		globals->virtual_address = _processes[index]->p_virtual_address;
    		globals->size = data_size;
		_processes[index]->variables.push_back(globals);

		_processes[index]->p_virtual_address +=  globals->size;
		number_of_pages = _processes[index]->p_virtual_address / pageTable->getPageSize();
		for(i=_processes[index]->_next_page; i<=number_of_pages; i++)
		{
			pageTable->addEntry(pid, i);
			_processes[index]->_next_page++;
			_processes[index]->variables[1]->page_numbers.push_back(i);
		}

		Variable *stack = new Variable();
		stack->name = "<STACK>";
    		stack->virtual_address = _processes[index]->p_virtual_address;
    		stack->size = 65536;
		_processes[index]->variables.push_back(stack);
		_processes[index]->p_virtual_address +=  stack->size;

		number_of_pages = _processes[index]->p_virtual_address / pageTable->getPageSize();
		for(i=_processes[index]->_next_page; i<=number_of_pages; i++)
		{
			pageTable->addEntry(pid, i);
			_processes[index]->_next_page++;
			_processes[index]->variables[2]->page_numbers.push_back(i);
		}

		_max_size -= _processes[index]->p_virtual_address;
		//std::cout <<"MAX size"<<_max_size<<"\n";
		//std::cout <<"next page"<<_processes[index]->_next_page<<"\n";

    		Variable *var = new Variable();
    		var->name = "<FREE_SPACE>";
    		var->virtual_address = -1;
    		var->size = _max_size - _processes[index]->p_virtual_address;
		_processes[index]->variables.push_back(var);
	}//free space
	return pid;
}
int Mmu::allocate( uint32_t pid, const std::string& var_name, const std::string& data_type, int num_element, PageTable *pageTable)
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
    
	_processes[index]->variables[var_index]->data.resize(num_element);
	//Resize to how many elements supposed to be able to hold.
	
	virtual_addr = _processes[index]->p_virtual_address;
	_processes[index]->p_virtual_address +=  _processes[index]->variables[var_index]->size;

	int number_of_pages = _processes[index]->p_virtual_address / pageTable->getPageSize();
	bool lop = false;
	for(int i=_processes[index]->_next_page; i<=number_of_pages; i++)
	{
		pageTable->addEntry(pid, i);
		_processes[index]->_next_page++;
		_processes[index]->variables[var_index]->page_numbers.push_back(i);
		lop = true;
	}
	if( number_of_pages < _processes[index]->_next_page && !lop)
	{ 
		_processes[index]->variables[var_index]->page_numbers.push_back(number_of_pages); 
	}
	//Track available space
	_max_size = _max_size - _processes[index]->p_virtual_address;

	//Add and Track FREE SPACE
	Variable *var = new Variable();
    	var->name = "<FREE_SPACE>";
    	var->virtual_address = _processes[index]->p_virtual_address;
    	var->size = _max_size - _processes[index]->p_virtual_address;
	_processes[index]->variables.push_back(var);

	return virtual_addr;
}
int Mmu::set(uint8_t *memory, uint32_t pid, std::string& var_name, uint32_t offset, std::vector<std::string> values, PageTable *pageTable)
{
	int index = findProcess(pid);
	if( index == -1 )
	{ 
		return -1; //error
	}
	int varAddr = findVariableAddr(var_name, index);
	if( varAddr == -1 )
	{ 
		return -1; //error
	}
	int physAddr = pageTable->getPhysicalAddress(pid, varAddr);

	int var_index = findVariableIndex(var_name, index);
	if( var_index == -1 )
	{ 
		return -1; //error
	}
	int addtlOffset = 0;

	
	uint8_t *pointer;
	for(int i=0; i<values.size(); i++)
	{
		std::vector<uint8_t> container( values[i].begin(), values[i].end() );
		//pointer = &container[0];
		_processes[index]->variables[var_index]->data[offset + addtlOffset] = values[i];


		pointer = (uint8_t*)(&values[i]);
		memory[physAddr] = *pointer;



		//memory[physAddr] = (uint8_t*)(values[i].c_str());
		//Still unsure how to exactly get the it to fit in here.
		addtlOffset++;
	}
	return 0;
}

int Mmu::free(uint32_t pid, std::string& var_name, PageTable *pageTable)
{
	int index = findProcess(pid);
	if( index == -1 )
	{ 
		return -1; //error
	}
	int var_index = findVariableIndex(var_name, index);
	if( var_index == -1 )
	{
		return -1; //error
	}

	_processes[index]->variables[var_index]->name = "<FREE_SPACE>"; //Set back to free.
	_processes[index]->variables[var_index]->virtual_address = -1; //Set to unavailable virtual address
	_processes[index]->variables[var_index]->size = 0; //No size allocated.
	
	//this is going to delete other varialbes located in same allocation, is that ok?
	for(int j=0; j<_processes[index]->variables[var_index]->page_numbers.size(); j++)
	{
		pageTable->eraseEntry( pid,  j );
		//_processes[index]->_next_page--;
	}

	return 0;
}
int Mmu::terminate(uint32_t pid)
{
	int pid_index = findProcess(pid);
	if( pid_index == -1 )
	{ 
		return -1; //error
	}

	_processes[pid_index]->variables.clear();//clear all varialbes
	//not sure
	_max_size = _max_size + _processes[pid_index]->p_virtual_address;
	delete _processes[pid_index];//delete the process
	
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

int Mmu::findVariableIndex(std::string& varName, int index){

	for(int i = 0; i < _processes[index]->variables.size(); i ++){

		if( _processes[index]->variables[i]->name.compare(varName) == 0){
			return i;
		}
	}
	return -1;
}

int Mmu::findVariableAddr(std::string& varName, int index){

	for(int i = 0; i < _processes[index]->variables.size(); i ++){

		if( _processes[index]->variables[i]->name.compare(varName) == 0){
			
			return _processes[index]->variables[i]->virtual_address;
		}
	}
	return -1;
}
void Mmu::print2()//just to chekc,
{
	int i, j, k;
    for (i = 0; i < _processes.size(); i++)
    {
        for (j = 0; j < _processes[i]->variables.size(); j++)
        {
		for(k=0; k<_processes[i]->variables[j]->page_numbers.size(); k++)
		{
			std::cout << _processes[i]->pid <<" "<< _processes[i]->variables[j]->name
					<< " : "<<_processes[i]->variables[j]->page_numbers[k] <<"\n";
		}
	}
    }
}
void Mmu::print()
{
    int i, j;
    std::stringstream hex;
    char space = ' ';

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
		hex.str(std::string());
		hex  << "0x"
			<< std::setfill('0')
			<< std::setw(8)
			<< std::uppercase
			<< std::hex
			<< _processes[i]->variables[j]->virtual_address;

	    	std::cout << _processes[i]->pid
	              << std::string( 2, space )
	    	      << "| " 
		      << _processes[i]->variables[j]->name
	              << std::string( 14 - _processes[i]->variables[j]->name.length(), space )
		      << "|   "
		      << hex.str()
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
void Mmu::printData(int pid, std::string& var_name)
{
	int index = findProcess(pid);
	int varAddr = findVariableAddr(var_name, index);
	int var_index = findVariableIndex(var_name, index);

	if( index!=-1 && var_index!=-1 )
	{
		std::vector<std::string>::iterator it;
        	std::vector<std::string> *data = &_processes[index]->variables[var_index]->data;
		int size = 0;
	
		for(it = data->begin(); it != data->end() && size<4 ; it++)
		{
			if( (*it).compare("")!=0 ){ std::cout << *it << ", "; }
			//std::cout << *it << ", ";
			size++;
		}
		if( _processes[index]->variables[var_index]->data.size() >4 )
		{
			std::cout << "... [" 
				  << _processes[index]->variables[var_index]->data.size()
				  << " items]";
		}
		std::cout << std::endl;
	}
}
