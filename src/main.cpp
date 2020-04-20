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
    Mmu *mmu = new Mmu(page_size);
    PageTable *page_table = new PageTable(page_size);
    //TEST:BEGIN
    mmu->createNewProcess(5992,564);
    mmu->createNewProcess(14788,296);
    mmu->print();
	int t = 0;
	int addr = 0;
	t = mmu->allocate( 1024, "point_x", "int", 1 );
	std::cout << t << "\n";
	addr = page_table->getPhysicalAddress(1024, t);
	t = mmu->allocate( 1024, "point_y", "int", 1 );
	std::cout << t << "\n";
	t = mmu->allocate( 1024, "name", "char", 256 );
	std::cout << t << "\n";
	t = mmu->allocate( 1024, "time", "long", 2 );
	std::cout << t << "\n";
	t = mmu->allocate( 1024, "data", "int", 2000 );
	std::cout << t << "\n";
	t = mmu->allocate( 1025, "temperature", "double", 1 );
	std::cout << t << "\n";
	t = mmu->allocate( 1025, "pressure", "double", 1 );
	std::cout << t << "\n";
    	mmu->print();
    //TEST:END

    std::vector<std::string> spliter;
    int size = 0;
    // Prompt loop
    std::string command;
    std::cout << "> ";
    std::getline (std::cin, command);
    while (command != "exit") {
        // Handle command
        // TODO: implement this!
	spliter = splitString(command, ' ');
	size = spliter.size();
	if( spliter[0].compare("create")==0 && size==3 )
	{
		std::cout << mmu->createNewProcess( std::stoi(spliter[1]), std::stoi(spliter[2]) )
			  <<"\n";
	}
	else if( spliter[0].compare("allocate")==0 && size==5 )
	{
		std::cout << mmu->allocate( static_cast<uint32_t>(std::stoul(spliter[1])), 
						spliter[2], spliter[3], std::stoi(spliter[4]) )
			  << "\n";
	}
	else if( spliter[0].compare("set")==0 && size>4 )
	{
		
	}
	else if( spliter[0].compare("print")==0 && size>1 )
	{
		if( spliter[1].compare("mmu")==0 && size==2 )
		{
			mmu->print();
		}
		else if( spliter[1].compare("page")==0 )
		{

		}
		else if( spliter[1].compare("processes")==0 )
		{
			mmu->printProcesses();
		}
		else{
			//print <pid> <var_name>
		}
		
	}
	else if( spliter[0].compare("free")==0 )
	{

	}
	else if( spliter[0].compare("terminate")==0 )
	{

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
