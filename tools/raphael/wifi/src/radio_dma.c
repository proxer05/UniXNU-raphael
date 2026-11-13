#include <stdio.h>
#include <string.h>
#include "pmem.h"
#include "radio_dma.h"

#define RDMA_UNIT       0x100u          /* 256 B, RDMA_ALIGN */
#define RDMA_MAX_SIZE   (16u << 20)     /* policy bound, not a DT size */

static uint64_t ar_pa, ar_iova, ar_size, ar_next;
static int ar_ok;

static uint64_t
rd_round(uint64_t v, uint64_t align)
{
	uint64_t r = v + align - 1;
	return r < v ? 0 : r & ~(align - 1);
}

int
rdma_init(uint64_t cpu_pa, uint64_t iova, uint64_t size)
{
	ar_ok = 0;
	if (cpu_pa == 0 || iova == 0 || size == 0 || size > RDMA_MAX_SIZE ||
	    (size & (RDMA_UNIT - 1)) || (cpu_pa & (RDMA_UNIT - 1)) || (iova & (RDMA_UNIT - 1)) ||
	    (iova >> 32)) {
		return -1;
	}
	ar_pa = cpu_pa;
	ar_iova = iova;
	ar_size = size;
	ar_next = 0;
	ar_ok = 1;
	return 0;
}

int
rdma_ready(void)
{
	return ar_ok;
}

int
rdma_alloc(uint64_t size, uint64_t align, uint64_t *cpu_pa, uint64_t *iova)
{
	if (!ar_ok || size == 0 || size > ar_size) {
		return -1;
	}
	if (align < RDMA_UNIT) {
		align = RDMA_UNIT;
	}
	if (align & (align - 1) || align > (1u << 20)) {
		return -1;
	}
	uint64_t at = rd_round(ar_next, align);
	uint64_t end = rd_round(size, RDMA_UNIT);
	if (at < ar_next || end < size || at > ar_size || end > ar_size - at) {
		return -1;              /* rounding overflowed or the arena is full */
	}
	ar_next = at + end;
	/* zero through pmem: the kernel zeroed the arena once, but a previous
	 * allocation in the same boot may have written here */
	static const uint8_t z[4096];
	for (uint64_t o = 0; o < end; o += sizeof(z)) {
		if (pm_wr(ar_pa + at + o, z, (size_t)((end - o) < sizeof(z) ? (end - o) : sizeof(z)))) {
			return -1;
		}
	}
	*cpu_pa = ar_pa + at;
	*iova = ar_iova + at;
	return 0;
}

uint64_t
rdma_pa_to_iova(uint64_t cpu_pa)
{
	if (!ar_ok || cpu_pa < ar_pa || cpu_pa >= ar_pa + ar_size) {
		return 0;
	}
	return ar_iova + (cpu_pa - ar_pa);
}

int
rdma_iova_to_pa(uint64_t iova, uint64_t *cpu_pa)
{
	if (!ar_ok || iova < ar_iova || iova >= ar_iova + ar_size) {
		return -1;
	}
	*cpu_pa = ar_pa + (iova - ar_iova);
	return 0;
}

int
rdma_iova_owned(uint64_t iova, uint64_t size)
{
	if (!ar_ok || size > ar_size || iova < ar_iova) {
		return 0;
	}
	return iova - ar_iova <= ar_size - size;
}

uint64_t
rdma_size(void) { return ar_size; }

uint64_t
rdma_used(void) { return ar_next; }