#include "mmu.h"
#include "pagetable.h"
#define STACK_SIZE 65536
Mmu::Mmu(int memory_size)
{
    _next_pid = 1024;
    _max_size = memory_size;
	_total_allocated = STACK_SIZE;
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

	//Check if there's enough space to create the new process.
	if(_total_allocated + text_size + data_size > _max_size || text_size > _max_size || data_size > _max_size){
		printf("This would exceed the maximum amount of ram available.\n");
		return 0;
	} else {

		_total_allocated = _total_allocated + text_size + data_size;

		
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
}
int Mmu::allocate( uint32_t pid, const std::string& var_name, const std::string& data_type, int num_element, PageTable *pageTable)
{
	int index = findProcess(pid);
	if( index == -1 ){ return -1; }//error

	int var_index = findFreeVar(index);
	if( var_index == -1 ){ return -1; }//error

	


	_processes[index]->variables[var_index]->name = var_name;
	_processes[index]->variables[var_index]->virtual_address = _processes[index]->p_virtual_address;

	int virtual_addr = 0;

	_processes[index]->variables[var_index]->data_type = data_type;
	//set size
	int data_size = getDataSize( _processes[index]->variables[var_index]->data_type );
	_processes[index]->variables[var_index]->size = data_size * num_element;
    
	//_processes[index]->variables[var_index]->data.resize(num_element);
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

	int multFactor;
	int index = findProcess(pid);
	if( index == -1 ){ return -1; }//error

	int varAddr = findVariableAddr(var_name, index);
	if( varAddr == -1 ){ return -1; }//error

	int physAddr = pageTable->getPhysicalAddress(pid, varAddr);

	int var_index = findVariableIndex(var_name, index);
	if( var_index == -1 ){ return -1; }//error

	int addtlOffset = 0, i=0;
	std::string data_type = _processes[index]->variables[var_index]->data_type;
	
	if( data_type.compare("char") == 0 )
	{
		multFactor = 1;
		std::vector<char> transfer;
		for(i=0; i<values.size(); i++){ transfer.push_back( *values[i].begin() ); }
		std::memcpy( &memory[physAddr+offset], transfer.data(), transfer.size() );
	}
	else if( data_type.compare("short") == 0 )
	{
		multFactor = 2;
		std::vector<short> transfer;
		for(i=0; i<values.size(); i++){ transfer.push_back( (short)std::stoi(values[i]) ); }
		std::memcpy( &memory[physAddr+offset*2], transfer.data(), 2*transfer.size() );
	}
	else if( data_type.compare("int") == 0 )
	{
		multFactor = 4;
		std::vector<int> transfer;
		for(i=0; i<values.size(); i++){ transfer.push_back( std::stoi(values[i]) ); }
		std::memcpy( &memory[physAddr+offset*4], transfer.data(), 4*transfer.size() );
	}
	else if( data_type.compare("float") == 0 )
	{
		multFactor = 4;
		std::vector<float> transfer;
		for(i=0; i<values.size(); i++){ transfer.push_back( std::stof(values[i]) ); }
		std::memcpy( &memory[physAddr+offset*4], transfer.data(), 4*transfer.size() );
	}
	else if( data_type.compare("long") == 0 )
	{
		multFactor = 8;
		std::vector<long> transfer;
		for(i=0; i<values.size(); i++){ transfer.push_back( std::stoll(values[i]) ); }
		std::memcpy( &memory[physAddr+offset*8], transfer.data(), 8*transfer.size() );
	}
	else if( data_type.compare("double") == 0 )
	{
		multFactor = 8;
		std::vector<double> transfer;
		for(i=0; i<values.size(); i++){ transfer.push_back( std::stod(values[i]) ); }
		std::memcpy( &memory[physAddr+offset*8], transfer.data(), 8*transfer.size() );
	}

	//track data in the memory.
	/*for(int i=0; i<values.size(); i++)
	{
		const char *currVal = values[i].c_str();

		if((offset + addtlOffset) % pageTable->_page_size < multFactor){
			addtlOffset = addtlOffset + (offset+ addtlOffset) % pageTable->_page_size;
		} //Take it to the next page if it's going to fall inbetween pages.
		memory[offset + addtlOffset] = *currVal;

		addtlOffset = addtlOffset + 1*multFactor;		

	}*/
	return 0;
}

int Mmu::free(uint32_t pid, std::string& var_name, PageTable *pageTable, uint8_t *memory)
{
	int index = findProcess(pid);
	if( index == -1 ){ return -1; }//error

	int var_index = findVariableIndex(var_name, index);
	if( var_index == -1 ){ return -1; }//error

	int varAddr = findVariableAddr(var_name, index);

	int physAddr = pageTable->getPhysicalAddress(pid, varAddr);
	std::string data_type = _processes[index]->variables[var_index]->data_type;
	int data_size = getDataSize( data_type );
	int num_of_elements = _processes[index]->variables[var_index]->size / data_size;


	for(int offset=0; offset<num_of_elements; offset++){ memory[physAddr+offset*data_size] = 0; }//reset values

	//this is going to delete other varialbes located in same allocation, is that ok?
	int page_number = 0;
	for(int j=0; j<_processes[index]->variables[var_index]->page_numbers.size(); j++)
	{
		page_number = _processes[index]->variables[var_index]->page_numbers[j];
	//	std::cout <<"PAGE NUM: "<< _processes[index]->variables[var_index]->page_number <<"\n";
		pageTable->eraseEntry( pid, page_number );
		//_processes[index]->_next_page--;
	}
	//std::cout <<"PAGE NUM: "<< _processes[index]->variables[var_index]->page_number <<"\n";
	//pageTable->eraseEntry( pid, _processes[index]->variables[var_index]->page_number );

	_processes[index]->variables[var_index]->name = "<FREE_SPACE>"; //Set back to free.

	if (_processes[index]->variables[var_index+1]->name == "<FREE_SPACE>"){
		_processes[index]->variables[var_index]->size = _processes[index]->variables[var_index]->size + _processes[index]->variables[var_index+1]->size;
		_processes[index]->variables.erase(_processes[index]->variables.begin() + var_index+1); //Expand size of previous 
	} else if(_processes[index]->variables[var_index-1]->name == "<FREE_SPACE>"){
		_processes[index]->variables[var_index-1]->size = _processes[index]->variables[var_index-1]->size + _processes[index]->variables[var_index]->size;
		_processes[index]->variables.erase(_processes[index]->variables.begin() + var_index); //Expand size of previous 
	}


	//_processes[index]->variables[var_index]->virtual_address = -1; //Set to unavailable virtual address
	//_processes[index]->variables[var_index]->size = 0; //No size allocated.

	return 0;
}
int Mmu::terminate(uint32_t pid)
{
	int pid_index = findProcess(pid);
	if( pid_index == -1 ){ return -1; }//error

	_processes[pid_index]->variables.clear();//clear all varialbes
	//not sure
	_max_size = _max_size + _processes[pid_index]->p_virtual_address;
	delete _processes[pid_index];//delete the process
	
	return 0;
}

int Mmu::findProcess(uint32_t pid)
{
    	for (int i = 0; i < _processes.size(); i++){ 
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

		if( _processes[index]->variables[i]->name.compare(varName) == 0)
		{ 
			return i; 
		}
	}
	return -1;
}

int Mmu::findVariableAddr(std::string& varName, int index){

	for(int i = 0; i < _processes[index]->variables.size(); i ++){

		if( _processes[index]->variables[i]->name.compare(varName) == 0)
		{
			return _processes[index]->variables[i]->virtual_address;
		}
	}
	return -1;
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
    for (int i = 0; i < _processes.size(); i++){ std::cout << _processes[i]->pid << std:: endl; }
}
void Mmu::printData(int pid, std::string& var_name, PageTable *pageTable, uint8_t *memory)
{
	int index = findProcess(pid);
	if( index==-1 ){ return; }
	int var_index = findVariableIndex(var_name, index);
	if( var_index==-1 ){ return; }
	int varAddr = findVariableAddr(var_name, index);

	int physAddr = pageTable->getPhysicalAddress(pid, varAddr);
	std::string data_type = _processes[index]->variables[var_index]->data_type;
	int data_size = getDataSize( data_type );
	int num_of_elements = _processes[index]->variables[var_index]->size / data_size;
	int limit = 0, i = 0;
	if( data_type.compare("char") == 0 )
	{
		char *elements;
		elements = (char *)malloc(sizeof(char)*num_of_elements);
		std::memcpy(elements, &memory[physAddr], _processes[index]->variables[var_index]->size);

		for( i=0; i<num_of_elements && limit<4; i++){ std::cout << elements[i] << ", "; limit++; }
		if( num_of_elements >= 4){ std::cout << "... [" << num_of_elements << " items]"; }
		delete elements;
	}
	else if( data_type.compare("short") == 0 )
	{
		short *elements;
		elements = (short *)malloc(sizeof(short)*num_of_elements);
		std::memcpy(elements, &memory[physAddr], _processes[index]->variables[var_index]->size);

		for( i=0; i<num_of_elements && limit<4; i++){ std::cout << elements[i] << ", "; limit++; }
		if( num_of_elements >= 4){ std::cout << "... [" << num_of_elements << " items]"; }
		delete elements;
	}
	else if( data_type.compare("int") == 0 )
	{
		int *elements;
		elements = (int *)malloc(sizeof(int)*num_of_elements);
		std::memcpy(elements, &memory[physAddr], _processes[index]->variables[var_index]->size);

		for( i=0; i<num_of_elements && limit<4; i++){ std::cout << elements[i] << ", "; limit++; }
		if( num_of_elements >= 4){ std::cout << "... [" << num_of_elements << " items]"; }
		delete elements;
	}
	else if( data_type.compare("float") == 0 )
	{
		float *elements;
		elements = (float *)malloc(sizeof(float)*num_of_elements);
		std::memcpy(elements, &memory[physAddr], _processes[index]->variables[var_index]->size);

		for( i=0; i<num_of_elements && limit<4; i++){ std::cout << elements[i] << ", "; limit++; }
		if( num_of_elements >= 4){ std::cout << "... [" << num_of_elements << " items]"; }
		delete elements;
	}
	else if( data_type.compare("long") == 0 )
	{
		long *elements;
		elements = (long *)malloc(sizeof(long)*num_of_elements);
		std::memcpy(elements, &memory[physAddr], _processes[index]->variables[var_index]->size);

		for( i=0; i<num_of_elements && limit<4; i++){ std::cout << elements[i] << ", "; limit++; }
		if( num_of_elements >= 4){ std::cout << "... [" << num_of_elements << " items]"; }
		delete elements;
	}
	else if( data_type.compare("double") == 0 )
	{
		double *elements;
		elements = (double *)malloc(sizeof(double)*num_of_elements);
		std::memcpy(elements, &memory[physAddr], _processes[index]->variables[var_index]->size);

		for( i=0; i<num_of_elements && limit<4; i++){ std::cout << elements[i] << ", "; limit++; }
		if( num_of_elements >= 4){ std::cout << "... [" << num_of_elements << " items]"; }
		delete elements;
	}
	std::cout << std::endl;//std::cout << (char)memory[physAddr+i*data_size] << ", ";//*data_size
}
int Mmu::getDataSize(std::string& data_type)
{
	if( data_type.compare("char") == 0 ){ return 1; }
	else if( data_type.compare("short") == 0 ){ return 2; }
	else if( data_type.compare("int") == 0 || data_type.compare("float") == 0 ){ return 4; }
	else if( data_type.compare("long") == 0 || data_type.compare("double") == 0 ){ return 8; }
	return -1;
}
