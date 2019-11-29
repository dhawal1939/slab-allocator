#include "slab_allocator.hpp"
#include "slab_allocator.cpp"

void kmem_cache_free(void *objp){
    slab_s* slab;
    void *rel_slabaddr=NULL;
    void *abs_slabaddr=NULL;
    void *cache_addr=NULL;
    kmem_cache_t* cache;
    int64_t obj_size,obj_indx;

    rel_slabaddr = (void *)(((long)objp >>12)<<12);

    abs_slabaddr = base_address +(long)rel_slabaddr ;
    slab = (slab_s*)abs_slabaddr;

    cache_addr = slab_to_cache_address[abs_slabaddr]; //taking absolute address as key

    obj_size = ((kmem_cache_t*)cache_addr)->obj_size;

    obj_indx = ((int64_t)objp - (int64_t)abs_slabaddr)/obj_size;
    
    bufctl_clear(slab->bufctl,obj_indx+1);

    cache = (kmem_cache_t*)cache_addr;
    slab->num_active--;
    cache->ref_count--;
    cache->active_objs--;

    if(slab->num_active==0){
        slab->slab_type = FREE;
         void* temp = (void*)(long(abs_slabaddr)<<12 - sizeof(slab_s));
        ((slab_list*)(cache->partial_lst))->erase((slab_s*)temp);
        ((slab_list*) (cache->free_lst))->insert((slab_s*)temp);
    }

} 