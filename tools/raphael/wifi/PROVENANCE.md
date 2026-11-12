# Vendored donor sources

| What | From |
| --- | --- |
| `src/*.c`, `src/*.h` (except `ath.c`/`ath.h`: not in the donor's build list, incomplete leftovers) | https://github.com/nithinyadavamaraboina-online/IOS-26-Hot `phone/wifi/*` at commit `d249939b9c36380afaee1a76cf167aeee8347475` (2026-11-01), copied verbatim (`build.sh` excluded) |
| Licence | MIT (donor `LICENSE`, (c) 2026 dipa-studio); upstream headers kept in every file |
| Donor context | userspace half of the donor Wi-Fi stack: `wland` drives WCN3990 over `/dev/pmem` (QRTR/SMEM/SMP2P, WLFW/QMI, rmtfs, pdmapper) and serves the kernel's station through `/dev/wlan` (`flamewlan.h`) |

Modifications to `src/` (board constants only, 2026-11-12; every value verified against
the phone's live device tree — see board/BOARD-PROFILE.md):

- `wland.c`: `MPSS_PA` 0x8dc00000 → **0x8e000000**, `MPSS_SIZE` 0x0a000000 → **0x09600000**
- `radio.c`: `POOL_PA` 0xE0000000 → **0x8a000000** (inside removed_regions; probe on first boot)
- `wlfw.c`: `MSA_PA` 0x9b000000 → **0x8bc00000** (`MSA_SIZE` 0x180000 already matched)

Everything else in `src/` is verbatim. Any further behaviour change must be recorded here.
