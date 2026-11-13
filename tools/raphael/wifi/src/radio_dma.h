/*
 * radio_dma: host memory for the WLAN radio with the CPU and device address
 * spaces kept apart. The kernel (/dev/wlan FW_IOCTL_DMA_ARENA) hands out one
 * kernel-owned arena with two bases:
 *
 *   cpu_pa  - physical address for /dev/pmem (CPU reads/writes)
 *   iova    - low device address (0xa0000000.. on raphael) the WCN3990 sees
 *
 * The offset is identical in both, but they are not interchangeable: PMEM and
 * every mem_rd/mem_wr take cpu_pa, every descriptor/ring/MAC-leg address takes
 * iova. Mixing them is the bug this module exists to prevent.
 *
 * No free path on purpose: the arena lives while the radio may be DMAing, and
 * a modem/WLFW restart in the same phone boot reuses the mappings. Sizes are
 * rounded to 256 B (CE descriptors are 16 B, rings 4 KiB aligned); allocation
 * is checked and fail-closed.
 */
#ifndef RADIO_DMA_H
#define RADIO_DMA_H
#include <stddef.h>
#include <stdint.h>

#define RDMA_ALIGN 256u

/* 0 = ok; -1 when the arena is missing/invalid (radio must not start) */
int rdma_init(uint64_t cpu_pa, uint64_t iova, uint64_t size);
int rdma_ready(void);

/* zeroed inside the arena; 0 = ok, -1 = out of memory or bad arguments */
int rdma_alloc(uint64_t size, uint64_t align, uint64_t *cpu_pa, uint64_t *iova);

/* cpu_pa + offset and iova + offset of the same buffer; 0 when not in arena */
uint64_t rdma_pa_to_iova(uint64_t cpu_pa);
int rdma_iova_to_pa(uint64_t iova, uint64_t *cpu_pa);

/* [iova, iova+size) inside the arena? */
int rdma_iova_owned(uint64_t iova, uint64_t size);

uint64_t rdma_size(void);
uint64_t rdma_used(void);

#endif