#include "slab_allocator.hpp"

void* kmem_init_helper(){

    kmem_cache_t* cache_cache = (kmem_cache_t*)get_page();

    cache_cache->partial_lst = (slab_s*)(get_page()+PAGE_SIZE-sizeof(slab_s));
    kmem_cache_t* kmem_cs = (kmem_cache_t*)((kmem_cache_t*)cache_cache+1);

    for(int i=3;i<13;i++)
    {
        kmem_cs[i].obj_size = pow(2,i);
    }

    cache_size_s* cache_slab = (cache_size_s*)((cache_size_s*)cache_cache->partial_lst);

    for(int i=0;i<10;i++){
        cache_slab[i].cache_size = pow(2,i+3);
        cache_slab[i].cachep = &kmem_cs[i+3];
    }
    return kmem_cs; 
}