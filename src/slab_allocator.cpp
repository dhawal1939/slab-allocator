#include "slab_allocator.hpp"

void kmem_cache_create(void *memory)
{
    kmem_cache_t * kmem_cs = (kmem_cache_s*)memory;
    for(int i=3;i<NO_OF_CACHES+3;i++)
    {
        kmem_cs[i].obj_size = pow(2,i);
        sprintf(kmem_cs[i].name, "%s-%d\0", "size", kmem_cs[i].obj_size);
        
        kmem_cs[i].ctor=kmem_cs[i].dtor=NULL;
       
        kmem_cs[i].partial_lst = (void*)new list<slab_s*>();
        kmem_cs[i].free_lst = (void*)new list<slab_s*>();
        kmem_cs[i].full_lst = (void*)new list<slab_s*>();
       
        //kmem_cs[i].next,kmem_cs[i].prev //update later not required

        kmem_cs[i].num_of_slabs = 0;
        
        //No of colors 
        kmem_cs[i].color = 3;
        // color_next is incremented when new slab is added, color_off is constant
        // color_next is updated till a it reaches the color size and rotates back to same offset 
        kmem_cs[i].color_next = 0;
        kmem_cs[i].color_off = 64;

        // Calculating Max Number of Objects in a Kmem_cache constant for all slabs in that cache
        int64_t color_and_slab_data = MAX_SLAB_SIZE + (kmem_cs[i].color-1)*kmem_cs[i].color_off;
        kmem_cs[i].max_objs_per_slab = floor((PAGE_SIZE- color_and_slab_data)/(float)kmem_cs[i].obj_size);

        kmem_cs[i].active_objs = kmem_cs[i].ref_count = 0;
    }
}

void* kmem_init_helper(){

    kmem_cache_t* cache_cache = (kmem_cache_t*)get_page();
    
    sprintf(cache_cache->name,"kmem-cache\0");
    
    cache_cache->partial_lst = (void*)new list<slab_s*>();
    cache_cache->free_lst = (void*)new list<slab_s*>();
    cache_cache->full_lst = (void*)new list<slab_s*>();

    ((list<slab_s*>*)(cache_cache->partial_lst))->push_back((slab_s*)(get_page()+PAGE_SIZE-sizeof(slab_s)));
    
    //hardcoded we can modify if time permits
    kmem_cache_t* kmem_cs = (kmem_cache_t*)((kmem_cache_t*)cache_cache+1);

    cache_size_s* cache_slab = (cache_size_s*)(base_address+PAGE_SIZE);

    kmem_cache_create(kmem_cs);

    for(int i=0;i<NO_OF_CACHES;i++)
    {
        cache_slab[i].cache_size = pow(2,i+3);
        cache_slab[i].cachep = &kmem_cs[i+3];
    }
    return kmem_cs; 
}

int64_t calculate_color_offset(kmem_cache_t *cachep)
{
    int64_t color_offset = -1;
    if(cachep)
    {
        color_offset = cachep->color_next*cachep->color_off;
        cachep->color_next++;
        cachep->color_next %= cachep->color;
    }
    return color_offset;
}

void kmem_init(void* basememory)
{
    base_address = basememory;
	kmem_cache_t* first_cache = (kmem_cache_t*)kmem_init_helper();
	kmem_cache_t* temp_cache=first_cache;
	for(int i = 0; i < NO_OF_CACHES; i++)
    {
		kmem_cache_grow(temp_cache);
        temp_cache++;
	}
}

void kmem_cache_grow(kmem_cache_t *cachep)
{

    //hash needs to be made so as to use it at delete
    if(cachep)
    {
        void *new_slab = get_page();
        cachep->num_of_slabs++;

        slab_s *slab_data = (slab_s*)(new_slab + PAGE_SIZE - sizeof(slab_data));

        slab_data->max_objects = cachep->max_objs_per_slab;

        slab_data->start_adrr = new_slab + calculate_color_offset(cachep);
        slab_data->slab_type = FREE;
        slab_data->num_active = 0;

        slab_data->free_adr = 0;

        slab_to_cache_address[new_slab] = cachep;
        ((list<slab_s*>*)cachep->free_lst)->push_back(slab_data);        
    }
    return;
}