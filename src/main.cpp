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

    //TEST:BEGIN
    /*
	mmu->createNewProcess(5992,564,page_table);
	page_table->print();

    mmu->createNewProcess(14788,296,page_table);
	page_table->print();

    mmu->print();
	int t = 0;
	int addr = 0;
	t = mmu->allocate( 1024, "point_x", "int", 1, page_table );

	addr = page_table->getPhysicalAddress(1024, t);

	t = mmu->allocate( 1024, "point_y", "int", 1, page_table );

	addr = page_table->getPhysicalAddress(1024, t);

	t = mmu->allocate( 1025, "temperature", "double", 1, page_table );

	addr = page_table->getPhysicalAddress(1025, t);

	t = mmu->allocate( 1024, "name", "char", 256, page_table );

	addr = page_table->getPhysicalAddress(1024, t);

	t = mmu->allocate( 1024, "time", "long", 2, page_table );

	addr = page_table->getPhysicalAddress(1024, t);

	t = mmu->allocate( 1024, "data", "int", 2000, page_table );

	addr = page_table->getPhysicalAddress(1024, t);

	t = mmu->allocate( 1025, "pressure", "double", 1, page_table );

	addr = page_table->getPhysicalAddress(1025, t);
    	mmu->print();


	std::string data2 = "data";
	std::string time = "time";
	std::string x = "point_x";
	int index = mmu->findProcess(1024);


	int address = mmu->findVariableAddr(x, index);
	int phy_address = page_table->getPhysicalAddress(1024, address);
	std::cout << "point_x vd: " <<  address << "  phy address: " << phy_address <<"\n";


	address = mmu->findVariableAddr(time, index);
	phy_address = page_table->getPhysicalAddress(1024, address);
	std::cout << "time vd: " <<  address << "  phy address: " << phy_address <<"\n";


	address = mmu->findVariableAddr(data2, index);
	phy_address = page_table->getPhysicalAddress(1024, address);
	std::cout << "data vd: " <<  address << "  phy address: " << phy_address <<"\n";


	std::string temperature = "temperature";
	std::string pressure = "pressure";
	index = mmu->findProcess(1025);


	address = mmu->findVariableAddr(temperature, index);
	phy_address = page_table->getPhysicalAddress(1025, address);
	std::cout << "temperature vd: " <<  address << "  phy address: " << phy_address <<"\n";


	address = mmu->findVariableAddr(pressure, index);
	phy_address = page_table->getPhysicalAddress(1025, address);
	std::cout << "pressure vd: " <<  address << "  phy address: " << phy_address <<"\n";


	//page_table->print();
	

	std::string name = "name";
	std::vector<std::string> values;
	values.push_back("l");
	values.push_back("o");
	values.push_back("c");
	values.push_back("a");
	values.push_back("t");
	values.push_back("i");
	values.push_back("o");
	values.push_back("n");
	mmu->set( memory, 1024, name, 0, values, page_table);
	mmu->printData(1024, name, page_table, memory);
	//mmu->print();
	//page_table->print();


	std::vector<std::string> values2;
	values2.push_back("9123456");
	
	mmu->set( memory, 1024, time, 0, values2, page_table);
	mmu->printData(1024, time, page_table, memory);

	std::cout << " T T \n";
	std::vector<std::string> values4;
	values4.push_back("9756311357");
	mmu->set( memory, 1024, time, 1, values4, page_table);
	mmu->printData(1024, time, page_table, memory);

	//std::vector<std::string> values3;
	//values3.push_back("98.6");
	//mmu->set( memory, 1025, temperature, 0, values3, page_table);
	//mmu->printData(1025, temperature, page_table, memory);

	//mmu->free( 1024, data2 , page_table, memory );
	//mmu->free( 1024, name , page_table, memory );
	//page_table->print();


	mmu->terminate(1025);
	mmu->print();

	*/


	
	/*std::string time = "time";
	std::vector<std::string> values2;
	values2.push_back("91235684");
	mmu->set( memory, 1024, time, 0, values2, page_table);
	std::vector<std::string> values3;
	values3.push_back("35764365486");
	mmu->set( memory, 1024, time, 1, values3, page_table);
	mmu->printData(1024, time);

	std::string data = "data";
	mmu->free( 1024, data , page_table );
	mmu->print();
	page_table->print();

	*/

	/*std::vector<std::string> values3;
	values.push_back("3214567");
	mmu->set( memory, 1025, time, 1, values3, page_table);*/
	//mmu->printData(1025, time);
	

	//mmu->terminate(1025);
    	//mmu->print();

	//page_table->print();
    //TEST:END

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
        // TODO: implement this!
	spliter = splitString(command, ' ');
	size = spliter.size();
	if( spliter[0].compare("create")==0 && size==3 )
	{
		text_size = std::stoi(spliter[1]);
		data_size = std::stoi(spliter[2]);
		std::cout << mmu->createNewProcess( text_size, data_size, page_table) << std::endl;
		
	}
	else if( spliter[0].compare("allocate")==0 && size==5 )
	{
		pid = static_cast<uint32_t>(std::stoul(spliter[1]));
		var_name = spliter[2];
		data_type = spliter[3];
		num_of_elements = std::stoi(spliter[4]);
		std::cout << mmu->allocate( pid, var_name, data_type, num_of_elements, page_table ) << std::endl;
	}
	else if( spliter[0].compare("set")==0 && size>4 )
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
	else if( spliter[0].compare("print")==0 && size>1 )
	{
		if( spliter[1].compare("mmu")==0 && size==2 )
		{
			mmu->print();
		}
		else if( spliter[1].compare("page")==0 )
		{
			page_table->print();
		}
		else if( spliter[1].compare("processes")==0 )
		{
			mmu->printProcesses();
		}
		else{
			pid = atoi(spliter[1].c_str());
			var_name = spliter[2];	
			mmu->printData(pid, var_name, page_table, memory);
		}
		
	}
	else if( spliter[0].compare("free")==0 && size==3 )
	{
		pid = atoi(spliter[1].c_str());
		var_name = spliter[2];
    		mmu->free(pid, var_name, page_table, memory);
	}
	else if( spliter[0].compare("terminate")==0 && size==2 )
	{
		pid = atoi(spliter[1].c_str());
		mmu->terminate( pid );
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
