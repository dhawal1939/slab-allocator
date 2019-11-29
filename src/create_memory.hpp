#ifndef CREATE_MEMORY_HPP_
#define CREATE_MEMORY_HPP_

#include "header.hpp"

void* memory;
int64_t *page_info;

void* create_memory(int64_t size);

void* get_page();

void* free_page();

#endif //!CREATE_MEMORT_HPP_