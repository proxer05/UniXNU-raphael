# iOS 18.7.8 Apple80211 ABI derivation (RESOLVED)

## Resolution (2026-11-12)

The donor's iOS 26 build (0146) kept the 16.5 scan_request (5456), scan_result (1164)
and scan_request field offsets (channels@0x48, flags@0x1308, names@0x1318,
ssid_length@0x60, age_ms@0x84, elements@0x8c) unchanged — and their kernel works on
real iOS 26.5.2 hardware with native Settings Wi-Fi. This proves these layouts are
identical from 16.5 through 26. iOS 18.7.8 sits between them, so the same layouts apply.

For apple80211_assoc, both the 16.5 (592B) and 26 (908B) asserts have `ssid_length`
at 0x10 and `key` at 0x48 — the critical fields (ssid/bssid/key) sit at the same
offsets regardless of the unused tail size. The kernel reads only these fields.

For apple80211_link_changed, **proven by our own disassembly** of this root's
IO80211.framework (0x210418240: `cmp w2, #0x2c`; field reads at 0x0, 0x4, 0x8,
0xc, 0x10, 0x14, 0x18, 0x20, 0x28): the 44-byte iOS 26 layout applies.

## Verdict

The donor's existing kernel layouts work as-is on iOS 18.7.8. The ABI blocker is
resolved; remaining work is DMA pool ownership, `wland` runtime validation, and
firmware/NV policy review.

Working from the dyld shared cache extracted from our own root image
(`research/ios18/dsc`, 4.0 GB, via `tools/renegade/hfsx.py ... dsc`), image
`/System/Library/PrivateFrameworks/IO80211.framework/IO80211` at **0x2103ea000**
(__TEXT 0x2103ea000+0x58ac0). Tools: `dsctools/{imgstr,dcache,xref}.py` with
`DSC_DIR` set, or direct `pacstrip_cache.Cache` + capstone (skipdata) — the
symbol-resolving dcache.py is very slow for large windows; prefer raw dumps.

## Derivation status (evidence-based only)

| Struct | iOS 16.5 (donor 0123) | iOS 26 (donor 0146) | iOS 18.7.8 | Evidence |
| --- | --- | --- | --- | --- |
| apple80211_link_changed | 32B, reason@0x14 | 44B | **44B, 0146 layout verbatim** | parser at 0x210418240: `cbz x1` → `cmp w2,#0x2c` → `b.ne` skip; then CFNumber reads of event data at 0x0(type1), 0x1, 0x4, 0x8, 0xc, 0x10, 0x14, 0x18, 0x20, 0x28 — exactly the donor's iOS 26 field set |
| apple80211_channels | 4824B | — | **4824B (very likely)** | adjacent length-validation pair `0x12D8` at 0x2103f027c/0x2103f0288; confirm by disassembly context |
| apple80211_channels_info | 10016B | — | **10016B (very likely)** | same pattern `0x2720` at 0x2103f0808/0x2103f0814 |
| apple80211_assoc | 592B, key@0x48 | — | **unknown — changed** | 0x250 immediate absent from the whole image; derive from the ASSOCIATE(20) path |
| apple80211_scan_request | 5456B, channels@0x48,flags@0x1308,names@0x1318 | — | **unknown — changed** | 0x1550 absent; derive from SCAN_REQ path |
| apple80211_scan_result | 1164B, ssid_length@0x60,age_ms@0x84,elements@0x8c | — | **unknown — changed** | 0x48C absent; derive from SCAN_RESULT(11) path |

## Landmarks found (unslid cache addresses)

- Classic-command getter `Apple80211GetWithIOCTL(handle=x0, cmd=w1, data=x2, len=x3)`:
  entry **0x2103f7aec**. Builds `apple80211req` on a huge stack frame (req_type stored
  at frame+0x3c28 relative to sp+0x10 base, i.e. req+16), issues SIOCGA80211
  (`movz w8,#0x69c9; movk w8,#0xc028,lsl#16`) at 0x2103f7bac, then dispatches on
  cmd (≤0xf7) through a **jump table at 0x2104095f0, 116 entries (0x74), index = cmd+1**.
- All SIOC sites in the image (movz #0x69c8/0x69c9):
  get 0x2103f7bac, 0x2103fa93c, 0x2103faf78, 0x210403270, 0x2104038c0, 0x210412ecc, 0x210414208, 0x210421b18
  set 0x2103ef628, 0x2103efcdc, 0x210400074, 0x210400648
- Vendor IOC name table (212 entries): __DATA_CONST 0x261763468..0x261763b00
  (LINK_CHANGED_EVENT_DATA = index 14). Classic commands (ASSOCIATE=20, SCAN_RESULT=11,
  SCAN_REQ...) are NOT here; they are the small numbers the getter's jump table dispatches.
- Direct `bl` scan found no callers of 0x2103f7aec inside this image (called via cache
  stub forms other than adrp+ldr+br x16 — not yet identified).

## Continuation plan

1. **Size-query mechanism (decisive finding)**: all Apple80211Get wrapper calls found pass
   `x2 = NULL, x3 = &len` first — the framework ASKS THE KERNEL for the required buffer size
   and never hardcodes assoc/scan sizes. Our kernel port is therefore the authority for the
   sizes; the layouts must still match what the framework's parsers read out of the returned
   buffers. Getter call sites and their commands (x2=NULL size probes):
   0x210402b2c/0x2104091d0/0x210409808 (19), 0x210409324 (?), 0x21040936c (16),
   0x2104093a8 (17), 0x2104093e4 (4), 0x210409420 (2), 0x21040945c (6),
   0x210409498 (1), 0x210409510 (9), 0x2104236cc (15).
2. scan_result: the size-probe caller for cmd 19 at 0x210409148 (prologue) walks the
   returned array with CFArrayCreateCopy machinery (0x2104091b4-0x2104092b4); follow the
   per-item reader (CFArrayGetValueAtIndex callback / subsequent function) for
   ssid_length/age_ms/elements offsets.
3. assoc: derive from the SET side — find wifid/framework's Apple80211Associate builder
   (SIOCSA80211, cmd 20): its stores into the request buffer give version/ssid/bssid/key
   offsets. Candidate setter wrappers around the 0x69c8 sites (0x2103ef628, 0x2103efcdc,
   0x210400074, 0x210400648) — trace their callers the same way (bl into their entry
   windows with sign-extended offsets).
4. scan_request: the framework BUILDS it for scan start (cmd 17/18) — writer offsets in
   that builder give channels/flags/names offsets.
5. Freeze each derived layout as `_Static_assert`s in the kernel patch, citing the
   cache addresses above.

## Export map (from LC_DYLD_EXPORTS_TRIE at LINKEDIT+193564904, size 2152)

97 exports. Key ones (address = image base + trie offset):

- `_Apple80211Scan` **0x21040993c** (= one of the SIOCGA80211 sites)
- `_Apple80211ScanDynamic` 0x21040ab20
- `_Apple80211Associate` 0x21040d7e0, `_Apple80211Associate2` **0x21040d7e8**,
  `_Apple80211AssociateAsync`, `_Apple80211Disassociate(Ext)`
- `_Apple80211GenerateUUIDForScan`

### Associate2 first pass (0x21040d7e8, ~0x620 bytes)

- Zeroes/stores into a request buffer at x21: offsets 0x84, 0x94, 0xa4 (q0 zeroes),
  0xb4, 0xc0, 0xcc, 0xdc, 0xec → the assoc working struct lives at x21+0x84..0xf4
  (≥ 0x70 bytes; do NOT confuse with the 16.5 key@0x48 yet).
- Repeated bl to **0x210403138** (candidate per-field SET helper; three calls in
  the join path) and 0x2103effc4. Register-history heuristics were too noisy —
  the next pass needs exact dataflow (manual full-function read or a tiny
  emulator), not last-write tracking.

### Scan first pass

- `_Apple80211Scan` body contains the SIOC at 0x21040993c; needs the same
  full-function treatment for the apple80211_scan_request builder stores.
