#ifndef SLAB_ALLOCATOR_H_
#define SLAB_ALLOCATOR_H_
#define slab_bufctl(slabp) \((kmem_bufctl_t *)(((slab_t*)slabp)+1))

#include "header.hpp"
#include "create_memory.hpp"

typedef struct slab_s;

typedef struct kmem_cache_s{
	const char* name;

	slab_s* free_lst;
	slab_s* partial_lst;
	slab_s* full_lst;

	int64_t obj_size;
	int64_t num_obj_slab;
	int64_t total_num_obj;
	int64_t ref_count;
	int64_t num_blocks_slab;
	
	int64_t color;
	int64_t color_off;
	int64_t color_next;

	void(*ctor)(void*);
	void(*dtor)(void*);

	kmem_cache_s* next;
	kmem_cache_s* prev;

}kmem_cache_t;

typedef int64_t kmem_bufctl_t;

struct slab_s{
   slab_s* slab_type;
   void* start_adrr;
   int64_t num_active;
   int64_t max_objects;
   kmem_bufctl_t free_adr;
   int64_t bufctl[16];
};

struct cache_size_s
{
	int64_t cache_size;
	kmem_cache_s *cachep;
};

int64_t base_address;

kmem_cache_t* kmem_cache_create(char*, int64_t, void(*)(void*,int64_t), void(*)(void*,int64_t));

void* kmem_cache_alloc(kmem_cache_t*);

int64_t kmem_cache_reap(int64_t);

int64_t kmem_cache_shrink(kmem_cache_t*);

void* kmem_cache_free(kmem_cache_t*, void*);

int64_t kmem_cache_destroy(kmem_cache_t*);

void *kmalloc(int64_t size);

int64_t kmem_cache_estimate(uint64_t slab_size);
/*
* kmem_caches implemented
* 4, 8, 16,32,64,128,256,512,1K, 2k,4k(max-page size)
*
* cache coloring - to be calculated on the go //do not include this function
* in header as its a helper function
*
* object1-slab-array-1 starts at 0, object1-slab-array-2 starts at L1_CACHE_LINE_SIZE
*
* calculate nearest 2 power function
* if the nearest 2 power goes higher than 4K allocate using kmalloc (preferable malloc need to discuss)
*
* think of shink routine which deallocates pages if the total count is way less than a page size
*
* need to implement a mini slabinfo to get summary of the memory
*
* sub task - cmake
*
* boundaries of one object slab can access other objects -- need to think about it as memory is contiguous and
* all objects are being made by single process and single pid(all memory is obtained at start and the memorys is assumed to be contiguous
* hence this discrepency) think of allocating pages on request when asked rather than creating whole memory before hand
*
*/

#endif //!SLAB_ALLOCATOR_H_
