#include "create_memory.hpp"

void *create_memory(int64_t size, uint64_t* page_info )
{
    int fd = open("/dev/zero", O_RDWR);
    void *memory = mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
    close(fd);

    page_info = (uint64_t*)calloc(sizeof(uint64_t), ceil(size/(float)getpagesize()));

    return memory;
}


void * get_page(void *memory, uint64_t *page_info)
{
    uint64_t empty_page = 0;
    uint64_t *traverse = page_info;
    while(traverse!=NULL)
    {
        if(*traverse == 0)
            break;
        empty_page++;
        traverse++;
    }
    return memory+getpagesize()*empty_page;
}

void* free_page(uint64_t *page_info, void * memory)
{
    return NULL;
}