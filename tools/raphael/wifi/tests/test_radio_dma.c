/* radio_dma.c: the CPU-PA / device-IOVA split of the WLAN arena. Host-only
 * tests with a fake pmem area (rdma_alloc zeroes through pmem). */
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "pmem.h"
#include "radio_dma.h"

static int failures;

static void
check(int ok, const char *what)
{
	printf("%s %s\n", ok ? "ok  " : "FAIL", what);
	if (!ok) failures++;
}

/* pmem fake: physical addresses map into a byte array at FAKE_BASE */
#define FAKE_BASE 0x140000000ull
uint8_t fake_dram[32 << 20];

static uint8_t *
fake_at(uint64_t pa, size_t len)
{
	if (pa < FAKE_BASE || pa - FAKE_BASE > sizeof(fake_dram) ||
	    len > sizeof(fake_dram) - (size_t)(pa - FAKE_BASE))
		return NULL;
	return fake_dram + (pa - FAKE_BASE);
}

int
pm_wr(uint64_t pa, const void *buf, size_t len)
{
	uint8_t *p = fake_at(pa, len);
	if (!p) return -1;
	memcpy(p, buf, len);
	return 0;
}
int
pm_rd(uint64_t pa, void *buf, size_t len)
{
	uint8_t *p = fake_at(pa, len);
	if (!p) return -1;
	memcpy(buf, p, len);
	return 0;
}
uint32_t rd32(uint64_t pa) { uint32_t v = 0; pm_rd(pa, &v, 4); return v; }
void wr32(uint64_t pa, uint32_t v) { pm_wr(pa, &v, 4); }
void wr64(uint64_t pa, uint64_t v) { wr32(pa, (uint32_t)v); wr32(pa + 4, (uint32_t)(v >> 32)); }
uint64_t pm_alloc(uint64_t size, uint64_t align) { (void)size; (void)align; return 0; }
int pm_free(uint64_t pa) { (void)pa; return -1; }
void pm_smc(uint64_t x[8]) { (void)x; }
int pm_list(struct pmem_list *l) { (void)l; return -1; }
int pm_rpmh(const char *res, unsigned n, const uint32_t *off, const uint32_t *val) { (void)res; (void)n; (void)off; (void)val; return -1; }
void delay_ms(unsigned ms) { (void)ms; }
int wait_bit(uint64_t pa, uint32_t mask, int set, unsigned ms) { (void)pa; (void)mask; (void)set; (void)ms; return -1; }

int
main(void)
{
	/* high cpu pa, unrelated low iova: the two must never be conflated */
	const uint64_t PA = 0x140000000ull + 0x200000;   /* high bank */
	const uint64_t IO = 0xa0000000ull;

	check(rdma_init(0, IO, 0x800000) != 0, "uninitialised: init rejects missing cpu pa");
	check(rdma_init(PA, 0, 0x800000) != 0, "init rejects missing iova");
	check(rdma_init(PA, IO, 0) != 0, "init rejects zero size");
	check(rdma_init(PA, IO, 0x800001) != 0, "init rejects unaligned size");
	check(rdma_init(PA | 0x80, IO, 0x800000) != 0, "init rejects unaligned cpu pa");
	check(rdma_init(PA, IO, 0x800000) == 0 && rdma_ready(), "init accepts the arena");

	uint64_t pa = 0, da = 0;
	check(rdma_alloc(0, 0x1000, &pa, &da) != 0, "alloc rejects zero size");
	check(rdma_alloc(0x10, 0x1000, &pa, &da) == 0, "alloc 16 bytes");
	check(pa == PA && da == IO, "first allocation is at the arena base");
	check(rdma_pa_to_iova(pa) == da, "pa->iova of the first allocation");
	uint64_t back = 0;
	check(rdma_iova_to_pa(da, &back) == 0 && back == pa, "iova->pa round trip");
	check(pa != da, "cpu pa and device address differ (high bank vs 0xa0000000)");

	check(rdma_alloc(0x100, 0x1000, &pa, &da) == 0, "second alloc");
	check(da == IO + 0x1000 && pa == PA + 0x1000, "second alloc is 4 KiB aligned after the first");
	check(rdma_pa_to_iova(pa) == da, "second pa->iova");

	/* zeroing: write a pattern, free-by-realloc does not exist, so check that
	 * an allocation zeroes its own range */
	memset(fake_dram + (size_t)(pa - FAKE_BASE), 0xff, 0x100);
	uint64_t p3 = 0, d3 = 0;
	check(rdma_alloc(0x100, 0x100, &p3, &d3) == 0, "third alloc");
	check(fake_dram[(size_t)(p3 - FAKE_BASE)] == 0, "allocations are zeroed through pmem");

	check(rdma_iova_to_pa(IO - 4, &back) != 0, "iova below the arena rejected");
	check(rdma_iova_to_pa(IO + 0x800000, &back) != 0, "iova at the arena end rejected");
	check(rdma_iova_owned(IO, 0x800000) == 1, "whole arena is owned");
	check(rdma_iova_owned(IO + 0x800000, 1) == 0, "one byte past the arena is not owned");
	check(rdma_iova_owned(IO, 0x800001) == 0, "a range crossing the end is not owned");
	check(rdma_pa_to_iova(PA - 0x1000) == 0, "pa outside the arena has no iova");
	check(rdma_pa_to_iova(PA + 0x800000) == 0, "pa at the arena end has no iova");

	/* exhaustion and overflow */
	check(rdma_alloc(0x800000, 0x1000, &pa, &da) != 0, "over-large alloc rejected");
	check(rdma_alloc(~0ull, 0x1000, &pa, &da) != 0, "overflowing alloc rejected");
	check(rdma_alloc(0x100, 0x2000, &pa, &da) == 0, "aligned alloc near the end");
	uint64_t used = rdma_used();
	check(used <= rdma_size(), "used never exceeds the arena");
	check(rdma_alloc(rdma_size() - used + 0x1000, 0x100, &pa, &da) != 0, "exact-end overrun rejected");

	/* a second init resets the bump pointer (a new arena is a new arena) */
	check(rdma_init(PA, IO, 0x800000) == 0, "re-init");
	check(rdma_used() == 0, "re-init resets the allocator");

	return failures ? 1 : 0;
}