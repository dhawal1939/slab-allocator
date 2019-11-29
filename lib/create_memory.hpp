#ifndef CREATE_MEMORY_HPP_
#define CREATE_MEMORY_HPP_

#include "header.hpp"


extern void* create_memory(int64_t size);

extern void* get_page();

extern void* free_page(int64_t);

#endif //!CREATE_MEMORT_HPP_
