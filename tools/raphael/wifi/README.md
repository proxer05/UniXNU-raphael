# Raphael Wi-Fi port (from IOS-26-Hot)

Port of the WCN3990 Wi-Fi stack of [IOS-26-Hot](https://github.com/nithinyadavamaraboina-online/IOS-26-Hot)
(MIT) to this repository's xnu-11215 / iOS 18.7.8 Raphael setup. Donor sources vendored
verbatim in `src/` from the pinned commit `d249939b9c36380afaee1a76cf167aeee8347475`
(`phone/wifi/*`); their upstream headers carry the copyright and the licence is MIT
(see `PROVENANCE.md`). Everything here is **off by default** until the port is complete
and tested on hardware; nothing in this directory is installed on the phone yet.

Status per phase (see the approved port plan):

- [x] A1 donor vendored (pinned commit, licence checked)
- [ ] A2 host tests green (`tests/run.sh`)
- [ ] B  kernel patch `xnu/patches-11215/0263-raphael-wifi-bridge.patch` (pmem + /dev/wlan,
      iOS 18 Apple80211 ABI derived from the local root's IO80211.framework — never copied
      from the donor's iOS 16/26 layouts)
- [ ] C  Raphael board profile with **verified** addresses (WLAN/CE/SMMU/MPSS/pool clear of
      the low-bank reserved-memory carve-outs) + firmware/NV from the phone's own partitions
- [ ] D  `wland` built for iOS 18, staged over USB SSH, bring-up ladder one step at a time

Known donor board assumptions that are NOT valid for Raphael until verified in phase C:
`MPSS_PA 0x8dc00000` (Realme), DMA pool `0xE0000000`, `SMMU_PA 0x15000000`, `WLAN_PA 0x18800000`.
The kernel's managed DRAM on Raphael is the high bank `0x140000000+0xc0000000`; the low bank
holds firmware pil regions and other carve-outs that must be enumerated first.

Host tests (`tests/`) compile the protocol sources on Linux with OpenSSL-backed shims for
CommonCrypto and a stub `sysctl`; they cover the `/dev/wlan` message ABI and the
parse/bring-up code paths that do not touch devices.
