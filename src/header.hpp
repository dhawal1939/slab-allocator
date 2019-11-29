#ifndef HEADER_HPP_
#define HEADER_HPP_

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cmath>

#include <unordered_map>
#include <list>

#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#define BITS 64
#define PAGE_SIZE 4096
#define CACHE_LINE 64
#define NO_OF_CACHES 10
using namespace std;

uint64_t bufctl_free(uint64_t *a,uint64_t max_objects){
	int i=15;
	while(~a[i]==0){
		i--;
	}
    //cout<<~a[i]<<endl;
    i=BITS-log2(~a[i])+(15-i)*BITS+1;
    //cout<<i<<endl;
	return (i<=max_objects)?i:-1; // no .of BITS in the data type.
}

void bufctl_set(uint64_t *a,uint64_t x){
	int i=16*BITS-x;
	a[i/BITS]|=(1<<(i%BITS));
}

void bufctl_clear(uint64_t *a,uint64_t x){
	int i=16*BITS-x;                                  // coming from MSB
	a[(i/BITS)]&=(~(1<<(i%BITS)));                    // array index and bit position
}
#endif //!HEADER_HPP
