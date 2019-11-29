#include "slab_allocator.hpp"

int main()
{
	void* baseaddress = create_memory(100 * 1024 * 1024);
	kmem_init(baseaddress);

	for(int i=3; i < NO_OF_CACHES + 3; i++)
	{
		for(int j=0; j < 100; j++)
		{
			kmalloc(pow(2,i));
		}
	}
	slab_info();
	return 0;
}
