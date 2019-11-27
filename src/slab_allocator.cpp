#include "slab_allocator.hpp"

void kmem_cache_create(void *memory)
{
    kmem_cache_t * kmem_cs = (kmem_cache_s*)memory;
    for(int i=3;i<13;i++)
    {
        kmem_cs[i].obj_size = pow(2,i);
        sprintf(kmem_cs[i].name, "%s-%d\0", "size", kmem_cs[i].obj_size);
        kmem_cs[i].ctor=kmem_cs[i].dtor=NULL;
        kmem_cs[i].free_lst = kmem_cs[i].full_lst=kmem_cs[i].partial_lst=NULL;
        //kmem_cs[i].next,kmem_cs[i].prev update later not required
        kmem_cs[i].num_blocks_slab = 1;
        kmem_cs[i].num_obj_slab = floor((PAGE_SIZE-sizeof(slab_s))/(float)kmem_cs[i].obj_size);
        kmem_cs[i].ref_count =0;
        // call color function 
        kmem_cs[i].color_next = 0;
        kmem_cs[i].color_off = 64;
        //color_next increment when new slab is added, color off is constant
        kmem_cs[i].total_num_obj=0;
        kmem_cs[i].active_objs=0;
    }
}

void* kmem_init_helper(){

    kmem_cache_t* cache_cache = (kmem_cache_t*)get_page();

    cache_cache->partial_lst = (slab_s*)(get_page()+PAGE_SIZE-sizeof(slab_s));
    kmem_cache_t* kmem_cs = (kmem_cache_t*)((kmem_cache_t*)cache_cache+1);

    cache_size_s* cache_slab = (cache_size_s*)((cache_size_s*)cache_cache->partial_lst);

    kmem_cache_create(kmem_cs);

    for(int i=0;i<10;i++){
        cache_slab[i].cache_size = pow(2,i+3);
        cache_slab[i].cachep = &kmem_cs[i+3];
    }
    return kmem_cs; 
}