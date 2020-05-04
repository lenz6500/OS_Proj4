#ifndef __PAGETABLE_H_
#define __PAGETABLE_H_

#include <iostream>
#include <string>
#include <map>
#include <vector>

class PageTable {
private:
    std::map<std::string, int> _table;
    std::vector<bool> *frame_table;

public:
    PageTable(int page_size);
    ~PageTable();
	int _page_size;

    void addEntry(uint32_t pid, int page_number);
    int getPhysicalAddress(uint32_t pid, int virtual_address);
    void print();
    void eraseEntry(uint32_t pid, int page_number);
    int getPageSize(){ return _page_size; }
};

#endif // __PAGETABLE_H_
