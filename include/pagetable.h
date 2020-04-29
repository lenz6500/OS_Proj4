#ifndef __PAGETABLE_H_
#define __PAGETABLE_H_

#include <iostream>
#include <string>
#include <map>
#include <vector>

class PageTable {
private:
    int _page_size;
    int _max_page_size;
    int _page_offset_bit;//page offset
    std::map<std::string, int> _table;
    std::vector<bool> *frame_table;

public:
    PageTable(int page_size);
    ~PageTable();

    void addEntry(uint32_t pid, int page_number);
    int getPhysicalAddress(uint32_t pid, int virtual_address);
    void print();
    void setMaxNumOfPage(int number_of_pages);
    void updateEntryTable();
    int getPageSize(){ return _page_size; }
    int getMaxPageSize(){ return _max_page_size; }
};

#endif // __PAGETABLE_H_
