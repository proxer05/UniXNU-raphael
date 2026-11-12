# Raphael radio board profile (Phase C)

Sources: LineageOS `android_kernel_xiaomi_sm8150` DTS (files kept here) **reconciled
against the phone's LIVE device tree** (read from `/proc/device-tree/reserved-memory`,
2026-11-12 — output archived in `live-dtb.txt`). Live values win wherever the two differ.

## Donor (Realme X2 Pro / Pixel 4) constants vs Raphael — final verdicts

| Constant | Donor | Raphael (LIVE confirmed) | Verdict |
| --- | --- | --- | --- |
| `WLAN_PA` | 0x18800000 | `icnss@18800000` (DTS) | ✅ keep |
| `SMMU_PA` | 0x15000000 | `apps-smmu@0x15000000` | ✅ keep |
| `APCS_IPC` | 0x17c0000c | `syscon@17c0000c` | ✅ keep |
| `MPSS_PA` | 0x8dc00000 | **0x8e000000 / 0x9600000** (live `modem_region`) | ❌ replace |
| WLAN MSA region | (pixel: fixed) | **0x8bc00000 / 0x180000** (live `pil_wlan_fw_region`) | ❌ replace |
| `POOL_PA` | 0xE0000000/16M | no independently reserved allocation established | unresolved; do not enable |

## LIVE reserved-memory map (all no-map; low bank ends 0x100000000)

| Region | Base | Size | End |
| --- | --- | --- | --- |
| hyp_mem | 0x85700000 | 0x600000 | 0x85d00000 |
| *(gap — xbl_dump in DTS, absent live: treat unknown)* | 0x85d00000 | 0x100000 | 0x85e00000 |
| xbl_aop | 0x85e00000 | 0x140000 | 0x85f40000 |
| smem | 0x86000000 | 0x200000 | 0x86200000 |
| **removed_regions (pool candidate)** | **0x86200000** | **0x5500000** | **0x8b700000** |
| camera_region | 0x8b700000 | 0x500000 | 0x8bc00000 |
| **pil_wlan_fw (WLAN MSA)** | **0x8bc00000** | **0x180000** | 0x8bd80000 |
| pil_npu | 0x8bd80000 | 0x80000 | 0x8be00000 |
| pil_adsp (live 0x2200000, DTS said 0x1a00000) | 0x8be00000 | 0x2200000 | 0x8e000000 |
| **pil_modem (MPSS)** | **0x8e000000** | **0x9600000** | 0x97600000 |
| pil_video (live resolves the DTS overlap: starts at modem end) | 0x97600000 | 0x500000 | 0x97b00000 |
| pil_slpi | 0x97b00000 | 0x1400000 | 0x98f00000 |
| pil_ipa_fw / gsi | 0x98f00000 | 0x15000 | 0x98f15000 |
| **pil_gpu zap = 0x98f15000** (matches patch 0261 — cross-validates the live read) | 0x98f15000 | 0x2000 | 0x98f17000 |
| pil_spss | 0x99000000 | 0x100000 | 0x99100000 |
| pil_cdsp (live base ≠ DTS) | 0x99100000 | 0x1400000 | 0x9a500000 |
| cont_splash / fb | 0x9c000000 | 0x2400000 | 0x9e400000 |
| qseecom | 0x9e400000 | 0x1400000 | 0x9f800000 |
| cdsp_sec | 0xa4c00000 | 0x3c00000 | 0xa8800000 |
| *(Android-usable low-bank RAM above)* | 0xa8800000 | — | ≤0x100000000 |

Kernel-managed DRAM under iOS: the **high bank only** (ADT `dram-base 0x140000000
+0xc0000000`); the whole low bank is outside the kernel's map, so pool/MSA/MPSS
regions live there via `/dev/pmem` device mappings, as the donor does.

## DMA pool: unresolved; do not write-probe reserved memory

`0x8a000000..0x8b000000` lies numerically within the live `removed_regions`
range, but that does **not** establish that it is unused, accessible to WLAN DMA,
or available to this kernel. Reserved/no-map memory can hold secure firmware or
peripheral allocations. Being outside the kernel-managed high bank is not proof
of ownership either. No read/write probe has been run, and none is authorized by
this profile.

The earlier recommendation to use that range as a pool was unsupported. The
userland source still contains this unverified candidate and must not be deployed.
Before enabling it, establish an explicitly reserved DMA allocation from the boot
firmware's complete memory map, account for firmware/secure ownership, and validate
the WLAN SMMU/cache/address-width requirements. The compile-only pmem patch rejects
physical transfers and SMC calls until a narrowly defined policy exists.

## Remaining Phase C inputs

- ~~radio firmware~~ **DONE** (2026-11-12): extracted from the stock Xiaomi raphael EEA
  V10.3.1.0 fastboot package (`NON-HLOS.bin`, FAT16 walked; `wland` reads lowercase
  names on the case-sensitive root): `modem.mdt`+b00–b31, `wlanmdsp.mbn` (4.0 MB, ELF),
  `bdwlan.*` board-data variants + `bdwlan.bin` — staged in `tools/raphael/wifi/firmware/`
  with hashes in `provenance.json`. No `mba.mbn` exists on this build; the donor's stage-3
  MBA reference needs mapping to this image set (or a newer package) before MPSS boot.
- First `sweetpmem=1` boot: verify the pool address (0x8a000000, 16 MiB inside
  removed_regions) with read/write probes before pointing anything at it.
- `modemst1/2`, `fsg`, `fsc` backups from the phone remain prudent before ANY radio
  bring-up (rmtfs write paths exist; audit before enabling).
