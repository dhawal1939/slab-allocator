#include "slab_allocator.hpp"

int main()
{
	void* baseaddress = create_memory(100 * 1024 * 1024);
	kmem_init(baseaddress);

	void *ptrs[10];

	int count = 0;

	for(int i=3; i < NO_OF_CACHES + 2; i++)
	{
		for(int j=0; j < 100; j++)
		{
			if(count<10 && pow(2,i)==2048)
			{
				ptrs[count] = kmalloc(pow(2,i));
				count++;
			}
			else
				kmalloc(pow(2,i));
		}
	}
	slab_info();

	printf("\n\n\nFreeing %d\n\n\n", count);
	for(int i = 0; i < count; i++)
		kfree(ptrs[i]);

	slab_info();

	return 0;
}
