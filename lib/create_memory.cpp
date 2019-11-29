#include "create_memory.hpp"

void *memory = NULL;
int64_t *page_info =  NULL;

void *create_memory(int64_t size)
{
    int fd = open("/dev/zero", O_RDWR);
    memory = mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, fd, 0);
    close(fd);

    page_info = (int64_t*)calloc(sizeof(int64_t), ceil(size/(float)PAGE_SIZE));

    return memory;
}

void* get_page()
{
    int64_t empty_page = 0;
    int64_t *traverse = page_info;
    while(traverse!=NULL)
    {
        if(*traverse == 0)
            break;
        empty_page++;
        traverse++;
    }
    page_info[empty_page] = 1;
    return memory + PAGE_SIZE * empty_page;
}

void* free_page(int64_t page_number)
{
	memset(memory + PAGE_SIZE * page_number , 0, PAGE_SIZE);
	page_info[page_number] = 0;
    return NULL;
}
