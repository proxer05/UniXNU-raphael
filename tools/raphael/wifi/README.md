# Raphael Wi-Fi port (from IOS-26-Hot)

Experimental backport of the WCN3990 userspace driver and XNU station bridge to
xnu-11215 / iOS 18.7.8. Donor source is pinned to
`d249939b9c36380afaee1a76cf167aeee8347475`; its MIT notice is in `LICENSE.donor`.
See `PROVENANCE.md` for local changes and upstream credits.

**Not a working or hardware-ready Wi-Fi port.** Do not deploy the experimental
`wland` build or enable the radio. Working GPU/jailbreak images are unchanged.

## Current checks and blockers

- Thirteen userspace protocol translation units compile in the Linux test harness.
  Its tests check the message structures and host crypto shims, not radio behavior.
- Patch `0263-raphael-wifi-bridge.patch` lists all six BSD sources and RedwoodWLAN.
  It reuses the existing SweetNetwork interface/stack classes rather than compiling
  colliding donor class definitions. Both Makefiles generate using XNU's actual
  config parser. Full compilation/linking still requires a successful macOS CI result.
- `/dev/pmem` and `/dev/wlan` default off. Driver opens enforce root credentials.
  Physical-memory transfers and SMC calls return `EPERM` pending a verified policy.
- iOS 18 association/scan buffer ABI derivation is incomplete. `ABI-NOTES.md` is
  exploratory evidence, not permission to choose unverified struct layouts.
- The live tree confirms MPSS `0x8e000000/0x9600000` and WLAN MSA
  `0x8bc00000/0x180000`, but does not prove a safe DMA pool. The candidate
  `0x8a000000` in the userspace source remains unverified; never write-probe it.
- A signed userspace binary built against hand-written Darwin headers is not yet
  runtime-valid: struct, constants, symbol versions and imports need an ABI audit.
- Firmware was extracted from the user's stock V10.3.1.0 package. Board calibration
  selection, firmware compatibility and nonpersistent NV behavior still need checks.

## Host checks

```sh
sh tools/raphael/wifi/tests/run.sh
python3 -I tools/raphael/wifi/tests/test_kernel_patch.py
```

No test enables firmware or contacts the phone. Test builds use temporary output
paths; the donated sources remain traceable to their original interfaces.

## Validation order

1. Source-only patch applies; configuration generation succeeds; macOS kernel CI
   compiles and links the entire disabled bridge.
2. Independently validate iOS 18 buffer layouts, userspace Darwin ABI and safe
   memory/SCM interfaces; firmware and NV inputs remain private.
3. Only then design separately authorized, staged on-device radio tests, with the
   working GPU/read-write boot image retained as rollback.

The presence of `/dev/wlan`, an `en0` object, a compiled binary or a green CI build
alone is not proof of scanning, association, packet traffic or native Settings support.
