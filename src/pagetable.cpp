#include "pagetable.h"
#include <math.h>
#include <sstream>
PageTable::PageTable(int page_size)
{
    _page_size = page_size;
    _page_offset_bit = log2( page_size );
    frame_table = new std::vector<bool>();
}

PageTable::~PageTable()
{
}

void PageTable::addEntry(uint32_t pid, int page_number)
{
    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);

    // Find free frame
    // TODO: implement this!

    std::vector<bool>::iterator it;
	std::cout << "SIZE: "<< frame_table->size() << std::endl;
    
    for (it = frame_table->begin(); it != frame_table->end(); it++)
    {
	if( *it == true )
	{ 
		//std::cout << *it << std::endl;
	}
    }

    int frame = frame_table->size();
    frame_table->push_back(true);
    _table[entry] = frame;
}

int PageTable::getPhysicalAddress(uint32_t pid, int virtual_address)
{
    	// Convert virtual address to page_number and page_offset

    	int page_number = virtual_address >> _page_offset_bit;
    	int page_offset = virtual_address & ((int)pow(2, _page_offset_bit)-1);

	printf("%d is page number;\n", page_number);
	printf("%d is page offset;\n", page_offset);

	//printf("%d is page number2;\n", virtual_address % _page_size );
	//printf("%d is page offset2;\n", virtual_address / _page_size );

    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);
    

    // If entry exists, look up frame number and convert virtual to physical address
    int address = -1;
    if (_table.count(entry) > 0)
    {
        // TODO: implement this!
    }

    return address;
}

void PageTable::print()
{
    std::map<std::string, int>::iterator it;
    char space = ' ';
    std::string token;
    int i = 0;

    std::cout << " PID  | Page Number | Frame Number" << std::endl;
    std::cout << "------+-------------+--------------" << std::endl;

    for (it = _table.begin(); it != _table.end(); it++)
    {
        // TODO: print all pages
	i=0;
	std::stringstream tokenStream(it->first);
	while(std::getline(tokenStream, token, '|')){
		if(!i)
		{
			std::cout << " " << token << " |";
		}
		else
		{
			std::cout << std::string( 12-token.length(), space ) << token << " |";
		}
		i++;
	}//while
	std::cout << std::string( 13-std::to_string(it->second).length(), space ) << it->second << std:: endl;
    }
}
