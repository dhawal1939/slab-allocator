#ifndef SLAB_ALLOCATOR_H_
#define SLAB_ALLOCATOR_H_


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct slab_s;

typedef struct kmem_cache_s{
	const char* name;

	slab_s* free_lst;
	slab_s* partial_lst;
	slab_s* full_lst;

	uint32_t obj_size;
	uint32_t num_obj_slab;
	uint32_t total_num_obj;
	uint32_t num_blocks_slab;

	void(*ctor)(void*);
	void(*dtor)(void*);

	kmem_cache_s* next;
	kmem_cache_s* prev;


}kmem_cache_t;

#endif //!SLAB_ALLOCATOR_H_
