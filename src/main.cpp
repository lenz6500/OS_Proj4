#include <iostream>
#include <string>
#include <math.h>
#include "mmu.h"
#include "pagetable.h"

void printStartMessage(int page_size);
std::vector<std::string> splitString(std::string text, char d);
int main(int argc, char **argv)
{
    // Ensure user specified page size as a command line parameter
    if (argc < 2)
    {
        fprintf(stderr, "Error: you must specify the page size\n");
        return 1;
    }

    // Print opening instuction message
    int page_size = std::stoi(argv[1]);
    printStartMessage(page_size);
    // Create physical 'memory'
    uint8_t *memory = new uint8_t[67108864]; // 64 MB (64 * 1024 * 1024)

    //Create MMU and PageTable
    Mmu *mmu = new Mmu(67108864);
    PageTable *page_table = new PageTable(page_size);
    int  number_of_pages;

    std::vector<std::string> spliter;
    int size = 0, text_size = 0, data_size = 0, num_of_elements = 0, offset = 0;
    uint32_t pid = 0;
    std::string var_name;
    std::string data_type;
    
    // Prompt loop
    std::string command;
    std::cout << "> ";
    std::getline (std::cin, command);
    while (command != "exit") {
        // Handle command
	spliter = splitString(command, ' ');
	size = spliter.size();
	if( spliter[0].compare("create")==0 && size==3 ) //create new process, automatically generate pid.
	{
		text_size = std::stoi(spliter[1]);
		data_size = std::stoi(spliter[2]);
		std::cout << mmu->createNewProcess( text_size, data_size, page_table) << std::endl;
		
	}
	else if( spliter[0].compare("allocate")==0 && size==5 ) //allocate command.
	{
		pid = static_cast<uint32_t>(std::stoul(spliter[1]));
		var_name = spliter[2];
		data_type = spliter[3];
		num_of_elements = std::stoi(spliter[4]);
		std::cout << mmu->allocate( pid, var_name, data_type, num_of_elements, page_table ) << std::endl;
	}
	else if( spliter[0].compare("set")==0 && size>4 ) //set value of variable in process.
	{
		std::vector<std::string> values;
		for(int j=4; j<spliter.size(); j++)
		{
			values.push_back(spliter[j]);
		}
		pid = static_cast<uint32_t>(std::stoul(spliter[1]));
		var_name = spliter[2];
		offset = stoi(spliter[3]);
		mmu->set(memory, pid, var_name, offset, values, page_table);
	}
	else if( spliter[0].compare("print")==0 && size>1 ) //Print function
	{
		if( spliter[1].compare("mmu")==0 && size==2 ) //Print mmu
		{
			mmu->print();
		}
		else if( spliter[1].compare("page")==0 ) //Print pagetable
		{
			page_table->print();
		}
		else if( spliter[1].compare("processes")==0 ) //Print all processes
		{
			mmu->printProcesses();
		}
		else if( size==2 )
		{
			std::vector<std::string> spliter2;//assume only get correst format
			spliter2 = splitString(spliter[1], ':'); //Print specific variable value
			
			pid = static_cast<uint32_t>(std::stoul(spliter2[0]));
			var_name = spliter2[1];
			mmu->printData(pid, var_name, page_table, memory);
		}
	}
	else if( spliter[0].compare("free")==0 && size==3 ) //Free variable or process
	{
		pid = atoi(spliter[1].c_str());
		var_name = spliter[2];
    		mmu->free(pid, var_name, page_table, memory);
	}
	else if( spliter[0].compare("terminate")==0 && size==2 ) //Terminate process
	{
		pid = atoi(spliter[1].c_str());
		mmu->terminate( pid, page_table );
	}
	else
	{
		//error
	}
        // Get next command
        std::cout << "> ";
        std::getline (std::cin, command);
    }
    return 0;
}

void printStartMessage(int page_size)
{
    std::cout << "Welcome to the Memory Allocation Simulator! Using a page size of " << page_size << " bytes." << std:: endl;
    std::cout << "Commands:" << std:: endl;
    std::cout << "  * create <text_size> <data_size> (initializes a new process)" << std:: endl;
    std::cout << "  * allocate <PID> <var_name> <data_type> <number_of_elements> (allocated memory on the heap)" << std:: endl;
    std::cout << "  * set <PID> <var_name> <offset> <value_0> <value_1> <value_2> ... <value_N> (set the value for a variable)" << std:: endl;
    std::cout << "  * free <PID> <var_name> (deallocate memory on the heap that is associated with <var_name>)" << std:: endl;
    std::cout << "  * terminate <PID> (kill the specified process)" << std:: endl;
    std::cout << "  * print <object> (prints data)" << std:: endl;
    std::cout << "    * If <object> is \"mmu\", print the MMU memory table" << std:: endl;
    std::cout << "    * if <object> is \"page\", print the page table" << std:: endl;
    std::cout << "    * if <object> is \"processes\", print a list of PIDs for processes that are still running" << std:: endl;
    std::cout << "    * if <object> is a \"<PID>:<var_name>\", print the value of the variable for that process" << std:: endl;
    std::cout << std::endl;
}
// Returns vector of strings created by splitting `text` on every occurance of `d`
std::vector<std::string> splitString(std::string text, char d)
{
    	std::vector<std::string> result;
	std::string token;
	std::stringstream tokenStream(text);
	while(std::getline(tokenStream, token, d)){
		result.push_back(token);
	}
    return result;
}
