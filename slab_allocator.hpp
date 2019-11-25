#ifndef SLAB_ALLOCATOR_H_
#define SLAB_ALLOCATOR_H_
#define slab_bufctl(slabp) \((kmem_bufctl_t *)(((slab_t*)slabp)+1))

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

typedef unsigned int kmem_bufctl_t;

struct slab_s{
   slab_s* list_type;
   void* start_adr;
   unsigned int num_active;
   kmem_bufctl_t free_adr;
};

#endif //!SLAB_ALLOCATOR_H_
