#ifndef CREATE_MEMORY_HPP_
#define CREATE_MEMORY_HPP_

#include "header.hpp"

void* create_memory(int32_t size);


void* get_page(uint32_t *page_info);


void* free_page(uint32_t *page_info, void * memory);

#endif //!CREATE_MEMORT_HPP_