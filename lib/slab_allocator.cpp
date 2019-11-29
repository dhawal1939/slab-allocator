#include "slab_allocator.hpp"
unordered_map<int64_t , void*> slab_to_cache_address;
void * base_address = NULL;
void bufctl_init(uint64_t* bufctl)
{
    memset(bufctl, 0, 16);
    return;
}

uint64_t bufctl_first_free(uint64_t *buffctl, uint64_t max_objects)
{
	int64_t i=7;
    	
	while(~buffctl[i] == 0)i--;
    
    i = BITS - log2(~buffctl[i]) + (7 - i) * BITS + 1;

	return (i <= max_objects) ? i : -1; 
}

void bufctl_set(uint64_t *buffctl,uint64_t index)
{
	int64_t i=8*BITS-index;
	uint64_t shift = 1;
	buffctl[i/BITS]|=(shift<<(i%BITS));
}

void bufctl_clear(uint64_t *buffctl,uint64_t index)
{
	// coming from MSB
	int64_t i = 8 * BITS - index;

	uint64_t shift = 1;
	// array index and bit position
	buffctl[(i/BITS)] &= (~(shift<<(i%BITS)));
}

void kmem_cache_create(void *memory)
{
//    kmem_cache_t *kmem_cs = (kmem_cache_t*)(base_address + sizeof(kmem_cache_t));
	kmem_cache_t *kmem_cs = (kmem_cache_t*)memory;
    for(int i=0;i<NO_OF_CACHES;i++)
    {
        kmem_cs[i].obj_size = pow(2,i+3);

        sprintf(kmem_cs[i].name, "%s-%ld", "size", kmem_cs[i].obj_size);
        
        kmem_cs[i].ctor = kmem_cs[i].dtor = NULL;
       
        kmem_cs[i].partial_lst = (void*)new slab_list();
        kmem_cs[i].free_lst = (void*)new slab_list();
        kmem_cs[i].full_lst = (void*)new slab_list();
       
        // No required in the implementation
        kmem_cs[i].next = kmem_cs[i].prev = NULL;

        kmem_cs[i].num_of_slabs = 0;
        
        //No of colors 
        kmem_cs[i].color = 3;
        // color_next is incremented when new slab is added, color_off is constant
        // color_next is updated till a it reaches the color size and rotates back to same offset 
        kmem_cs[i].color_next = 0;
        kmem_cs[i].color_off = 64;

        // Calculating Max Number of Objects in a Kmem_cache constant for all slabs in that cache
        int64_t color_and_slab_data = MAX_SLAB_SIZE + (kmem_cs[i].color-1)*kmem_cs[i].color_off;
        kmem_cs[i].max_objs_per_slab = floor((PAGE_SIZE - color_and_slab_data)/(float)kmem_cs[i].obj_size);

        kmem_cs[i].active_objs = kmem_cs[i].ref_count = 0;
    }
    return;
}

void* kmem_init_helper(){

    kmem_cache_t* cache_cache = (kmem_cache_t*)get_page();
    
    sprintf(cache_cache->name,"kmem-cache");
    
    // Calculate slab_data from page start 
    cache_cache->partial_lst = (void*)new slab_list();
    cache_cache->free_lst = (void*)new slab_list();
    cache_cache->full_lst = (void*)new slab_list();
    
    cache_cache->ctor = cache_cache->dtor = NULL;

    cache_cache->obj_size = sizeof(cache_size_s);

    cache_cache->num_of_slabs = 1;

    cache_cache->max_objs_per_slab = NO_OF_CACHES;
    
    cache_cache->next = cache_cache->prev = NULL;
    
    cache_cache->active_objs = cache_cache->ref_count = NO_OF_CACHES;
    
    cache_cache->color_off = cache_cache->color_next = cache_cache->color = 0;
    

    // Cache Slabs Meta Data
    slab_s *cache_slab_s = (slab_s*)(get_page()+PAGE_SIZE-sizeof(slab_s));
    
    cache_slab_s->slab_type = PARTIAL;

    cache_slab_s->max_objects = cache_slab_s->num_active = cache_cache->max_objs_per_slab;
    
    cache_slab_s->start_adrr = base_address+PAGE_SIZE;
    
    (*(slab_list*)(cache_cache->partial_lst)).insert(cache_slab_s);
    
    // hard Coded we can modify if time permits
    kmem_cache_t* kmem_cs = (kmem_cache_t*)(base_address + sizeof(kmem_cache_t));

    cache_size_s* cache_slab = (cache_size_s*)(base_address + PAGE_SIZE);

    kmem_cache_create(kmem_cs);
    kmem_cs = (kmem_cache_t*)(base_address + sizeof(kmem_cache_t));

    for(int i=0;i<NO_OF_CACHES;i++)
    {
        cache_slab[i].cache_size = pow(2,i+3);
        cache_slab[i].cachep = (kmem_cache_t*)(base_address + sizeof(kmem_cache_t)*(i+1));
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

void kmem_cache_grow(kmem_cache_t *cachep)
{
    if(cachep)
    {
        void *new_slab = get_page();

        cachep->num_of_slabs++;

        slab_s *slab_data = (slab_s*)(new_slab + PAGE_SIZE - sizeof(slab_s));

        slab_data->max_objects = cachep->max_objs_per_slab;

        slab_data->start_adrr = new_slab + calculate_color_offset(cachep);

        slab_data->slab_type = FREE;
        
        slab_data->num_active = 0;

        slab_data->free_adr = 0;

        bufctl_init(slab_data->bufctl);

        // hash needs to be made so as to use it at delete
        slab_to_cache_address[(int64_t)new_slab] = cachep;
    
        ((slab_list*)cachep->free_lst)->insert(slab_data);        
    }
    return;
}

void kmem_init(void* basememory)
{
    base_address = basememory;
	kmem_cache_t* first_cache = (kmem_cache_t*)kmem_init_helper();
	kmem_cache_t* temp_cache = first_cache;
	for(int i = 0; i < NO_OF_CACHES; i++)
    {
		kmem_cache_grow(temp_cache);
        temp_cache++;
	}
}

kmem_cache_t* kmem_cache_estimate(int64_t size)
{
    kmem_cache_t *cache_ptr = NULL;
    
    if(base_address)
    {
        int64_t index = ceil(log2(size));
        cache_ptr = (((kmem_cache_t*)base_address) + (index - 2));
    }

    return cache_ptr;
}

/* 
* We are only pushing the slab_s slab_data to lists free empty and full
* We need to calculate the slabs base address when required to access the slab from the slab_data
* 
*/

void* kmem_cache_alloc(kmem_cache_t *cachep)
{
    void* allocated = NULL;
    if(cachep)
    {
        int64_t obj_size = cachep->obj_size;
        
        slab_list  *partialist, *freelist, *fulllist;
        
        partialist = ((slab_list*)(cachep->partial_lst));
        fulllist = ((slab_list*)(cachep->full_lst));
        freelist = ((slab_list*)(cachep->free_lst));

        slab_s* free_partial_slab = NULL;

        if(partialist->size() != 0)
        {
        	free_partial_slab = *(partialist->begin());
        }
        else if(partialist->size()==0 && freelist->size()==0)
        {
            kmem_cache_grow(cachep);
            free_partial_slab = *(freelist->begin());
            freelist->erase(freelist->begin());

            free_partial_slab->slab_type = PARTIAL;
            partialist->insert(free_partial_slab);

        }
        else if(freelist->size() != 0)
        {
        	free_partial_slab = *(freelist->begin());
        	freelist->erase(free_partial_slab);

        	free_partial_slab->slab_type = PARTIAL;
        	partialist->insert(free_partial_slab);
        }

        // first free object based on bufctl bitvector
        uint64_t index = bufctl_first_free(free_partial_slab->bufctl, free_partial_slab->max_objects);
        
        bufctl_set(free_partial_slab->bufctl, index);
        
        index--;

        free_partial_slab->num_active++;

        // Allocated address starts from start_addr + index * object size
        
        allocated = (free_partial_slab->start_adrr + index * obj_size);
        
        if(free_partial_slab->num_active >= free_partial_slab->max_objects)
        {
            partialist->erase(partialist->find(free_partial_slab));

            fulllist->insert(free_partial_slab);
            free_partial_slab->slab_type = FULL;
        }

    }
    else
    {
        printf("CACHEP NOT ALLOCATED\n");
    }
    return allocated;
}



void * kmalloc(int64_t size)
{
    void *allocated = NULL;
    if(base_address)
    {
        kmem_cache_t* cachep = kmem_cache_estimate(size);

        allocated = kmem_cache_alloc(cachep);

        cachep->active_objs = ++cachep->ref_count;
    }
    else
    {
        printf("NO MEMORY ALLOCATED\n");
    }
    return allocated;
}



void kmem_cache_free(kmem_cache_t* cachep, void *objp, void* slab_base)
{
    if(cachep && objp && slab_base)
    {

        slab_s* slab_data = (slab_s*)(slab_base + PAGE_SIZE - sizeof(slab_s));

        int64_t obj_size,obj_indx;
        obj_size = cachep->obj_size;
        obj_indx = (((int64_t)objp - (int64_t)slab_data->start_adrr)/obj_size);

        bufctl_clear(slab_data->bufctl, obj_indx+1);

        slab_data->num_active--;
        cachep->active_objs = --cachep->ref_count;

        if(slab_data->slab_type == FULL && slab_data->num_active != 0)
        {
            slab_data->slab_type = PARTIAL;
            
            slab_list* full_list = (slab_list*)cachep->full_lst;
            slab_list* partial_list = (slab_list*)cachep->partial_lst;

            full_list->erase(full_list->find(slab_data));
            partial_list->insert(slab_data); 
        }
        else if(slab_data->slab_type == FULL && slab_data->num_active == 0)
        {
        	slab_data->slab_type = FREE;

        	slab_list* full_list = (slab_list*)cachep->full_lst;
			slab_list* free_list = (slab_list*)cachep->free_lst;

			full_list->erase(full_list->find(slab_data));
			free_list->insert(slab_data);

			if(free_list->size() > 2)
			{
				//REAPING WHEN CONDITION IS SATISFIED;
				slab_s* delete_slab = *(free_list->begin());
				free_list->erase(free_list->begin());
				cachep->num_of_slabs--;
				free_page(((int64_t)slab_base - (int64_t)base_address)/PAGE_SIZE);
			}
        }
        else if(slab_data->slab_type == PARTIAL && slab_data->num_active==0)
        {
            slab_data->slab_type = FREE;

            slab_list* partial_list = (slab_list*)cachep->partial_lst;
            slab_list* free_list = (slab_list*)cachep->free_lst;

            partial_list->erase(partial_list->find(slab_data));
            free_list->insert(slab_data);

            // STUB for REAP IF slab_FREE is more than 2 size
        }
        else
        {
            printf("SOME ERROR IN KMEM PARTIAL TO FREE / FULL TO PARTIAL\n");
        }
    }
    else
    {
        printf("ERROR IN KMEME CACHE FREE\n");
    }
    
    return ;
}

void kfree(void* obj){
    if(!obj) 
    {
        printf("NULL MEMORY PASSED\n");
        return;
    }

    void *slab_base=NULL;

    int64_t addr =  ((int64_t)obj - (int64_t)base_address)/PAGE_SIZE;

    slab_base = base_address + PAGE_SIZE*addr;
    
    kmem_cache_t *cachep=NULL;

    // taking absolute address as key
    cachep = (kmem_cache_t*)slab_to_cache_address[(int64_t)slab_base];
   
    kmem_cache_free(cachep, obj, slab_base);
}

void slab_info()
{
    kmem_cache_t* cachep = (kmem_cache_t*)base_address;

    char* pad = (char*)calloc(sizeof(char), 10);
    printf("NAME\t\tOBJ_SIZE\t\tMAX_OBJ_SLAB\t\tREF_COUNT\t\tNUM_OF_SLABS\t\tFREE_LIST SIZE\t\FULL_LIST SIZE\t\tPARTIAL_LIST SIZE\n\n");
    for(int i = 0; i < NO_OF_CACHES; i++)
    {
    	if(strlen(cachep[i].name)<=7)
    		printf("%s\t\t\t", cachep[i].name);
    	else
    		printf("%s\t\t", cachep[i].name);
        printf("%ld\t\t", cachep[i].obj_size);
        printf("%ld\t\t", cachep[i].max_objs_per_slab);
        printf("%ld\t\t", cachep[i].ref_count);
        printf("%ld\t\t", cachep[i].num_of_slabs);
        printf("%ld\t\t", ((slab_list*)(cachep[i].free_lst))->size());
        printf("%ld\t\t", ((slab_list*)(cachep[i].full_lst))->size());
        printf("%ld\n", ((slab_list*)(cachep[i].partial_lst))->size());
    }
    return;
}
