# Vendored donor sources

| What | From |
| --- | --- |
| `src/*.c`, `src/*.h` (except `ath.c`/`ath.h`: not in the donor's build list, incomplete leftovers) | https://github.com/nithinyadavamaraboina-online/IOS-26-Hot `phone/wifi/*` at commit `d249939b9c36380afaee1a76cf167aeee8347475` (2026-11-01), copied verbatim (`build.sh` excluded) |
| Licence | MIT (donor `LICENSE`, (c) 2026 dipa-studio); upstream headers kept in every file |
| Donor context | userspace half of the donor Wi-Fi stack: `wland` drives WCN3990 over `/dev/pmem` (QRTR/SMEM/SMP2P, WLFW/QMI, rmtfs, pdmapper) and serves the kernel's station through `/dev/wlan` (`flamewlan.h`) |

Modifications to `src/` (2026-11-12/13):

Board constants (MPSS/MSA ranges observed in the live device tree):

- `wland.c`: `MPSS_PA` 0x8dc00000 → **0x8e000000**, `MPSS_SIZE` 0x0a000000 → **0x09600000**
- `wlfw.c`: `MSA_PA` 0x9b000000 → **0x8bc00000** (`MSA_SIZE` 0x180000 already matched)

Host DMA (2026-11-13, replaces the donor's hardcoded physical pool):

- The donor's `POOL_PA` (0xe0000000, safe only because donor XNU excluded that RAM)
  and this port's 0x8a000000 guess are both gone. `radio.c` now takes its buffers
  from `/dev/wlan` `FW_IOCTL_DMA_ARENA`: kernel-owned memory with a low WLAN IOVA
  (raphael DT: `smmu_iova_base` 0xa0000000), exactly the CPU-PA/device-IOVA split
  Linux uses. New `radio_dma.c/.h` keeps the two address spaces apart; no physical
  pool or reserved-memory write remains.
- `radio.c`: `pool_alloc` → `rdma_alloc`; device-facing fields use the IOVA, CPU
  access keeps the PA; `htt_rx` validates the firmware-returned IOVA against the
  owned HTT RX region before mapping it back.
- `flamewlan.h`: adds `FW_IOCTL_SMMU_STATUS` and `FW_IOCTL_DMA_ARENA`
  (mirrors the kernel's `bsd/sys/flame_wlan.h`).

Everything else in `src/` is verbatim radio/protocol code. Any further behaviour
change must be recorded here.
