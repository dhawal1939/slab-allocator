#ifndef SLAB_ALLOCATOR_H_
#define SLAB_ALLOCATOR_H_


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
//#include 

typedef struct slab_t;

typedef struct kmem_cache_s
{
	slab_t *full;
	slab_t *partial;
	slab_t *free;
	uint32_t object_size;
	uint32_t number_object_size;
	uint32_t total_number_objects;
	uint32_t number_of_blocks_in_slab;

};
#endif //!SLAB_ALLOCATOR_H_
