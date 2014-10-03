#include <stdlib.h>
#include <stdio.h>

static int total = 0;

struct st_memblk {
	void *mb_base;
	size_t mb_size;
} memblock[512];

static int nmbs = 0;

void *dmalloc(size_t size)
{
	if (nmbs > sizeof(memblock)) {
		fprintf(stdout, "dmalloc: ++ Cannot allocate more space: not enough free blocks of memory (nmbs=%d)\n", nmbs);
		exit(0);
	}

	void *alloced = malloc(size);

	memblock[nmbs].mb_base = alloced;
	memblock[nmbs].mb_size = size;
	++nmbs;

	fprintf(stdout, "dmalloc: ++ allocating %d bytes\n", size);
	total += size;
	fprintf(stdout, "dmalloc: ++ total memory usage %d bytes\n", total);
	return alloced;
}

void dfree(void *p)
{
	int i = 0;
	for (; i < nmbs; i++) {
		if (memblock[i].mb_base == p) {
			printf("dfree: ++ unlocking 0x%x, %d bytes\n", (unsigned)p,
				memblock[i].mb_size);
			printf("dfree: ++ still allocated %d bytes\n",
				total -= memblock[i].mb_size);
			break;
		}
	}
	free(p);
}
