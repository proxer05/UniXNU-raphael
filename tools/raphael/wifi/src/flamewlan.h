/*
 * /dev/wlan: the kernel's Wi-Fi station (bsd/dev/flame_wlan.c, under en0 and wifid's Apple80211 requests,
 * ios-topaz 0900's redwood code) and the radio, which runs in userland (phone/wifi: wland). The same file is
 * the kernel's bsd/sys/flame_wlan.h.
 *
 * One message per read() or write(): a header, then `len` bytes. wland opens the device once the radio is up,
 * says HELLO, and from then on reads the kernel's requests (non-blocking) and writes what happened.
 */
#ifndef FLAMEWLAN_H
#define FLAMEWLAN_H
#include <stdint.h>
#include <sys/ioccom.h>

/* wland -> kernel: does the WLAN SMMU stream exist? out: uint32_t, 1 = translating */
#define FW_IOCTL_SMMU_STATUS    _IOR('W', 1, uint32_t)

/*
 * wland -> kernel: the WLAN DMA arena. Kernel-owned physical memory mapped at
 * low WLAN IOVAs by the dedicated apps-SMMU bank; the offset is identical in
 * both addresses (cpu_pa + off / iova + off). Root only; ENXIO when DMA setup
 * failed, in which case the radio must not start.
 */
struct fw_dma_arena {
	uint64_t pa;		/* CPU physical base (for /dev/pmem) */
	uint64_t iova;		/* device address base (for descriptors/rings) */
	uint64_t size;		/* bytes, 4 KiB aligned */
};
#define FW_IOCTL_DMA_ARENA      _IOR('W', 2, struct fw_dma_arena)

/* wland -> kernel: the WLAN bank's last context fault (diagnosis after an AXI error) */
struct fw_smmu_fault {
	uint32_t fsr;		/* context fault status */
	uint32_t fsynr0;	/* fault syndrome (access type) */
	uint32_t far_lo;	/* fault address, low */
	uint32_t far_hi;	/* fault address, high */
};
#define FW_IOCTL_SMMU_FAULT     _IOR('W', 3, struct fw_smmu_fault)

struct fw_hdr {
	uint16_t type;
	uint16_t len;			/* of the payload after the header */
};

enum {
	/* kernel -> wland */
	FW_SCAN = 1,			/* no payload: scan, then BSS_BEGIN, BSS..., SCAN_DONE */
	FW_JOIN = 2,			/* struct fw_join; JOIN_DONE when frames can pass or why not */
	FW_LEAVE = 3,			/* no payload; LEFT */
	FW_TX = 4,			/* an Ethernet frame (destination, source, type, payload) */

	/* wland -> kernel */
	FW_HELLO = 16,			/* struct fw_hello: the radio is up */
	FW_BSS_BEGIN = 17,		/* no payload: a new list of what was heard follows */
	FW_BSS = 18,			/* struct fw_bss */
	FW_SCAN_DONE = 19,		/* int32_t: 0 or an errno */
	FW_JOIN_DONE = 20,		/* int32_t: 0 or an errno */
	FW_LEFT = 21,			/* int32_t */
	FW_LINK = 22,			/* struct fw_link: frames can pass now (up), or no longer (left or lost) */
	FW_RX = 23,			/* an Ethernet frame */
};

#define FW_FRAME_MAX	1514
#define FW_MSG_MAX	(sizeof(struct fw_hdr) + 1600)

struct fw_hello {
	uint8_t address[6];
};

enum { FW_KEY_NONE, FW_KEY_PASSPHRASE, FW_KEY_PMK };

struct fw_join {
	uint8_t ssid[32], ssid_len;
	uint8_t bssid[6], any_bssid;	/* any_bssid: the access point of that name heard best */
	uint8_t key_kind, key_len;	/* FW_KEY_*: the passphrase (8-63 bytes) or the 32-byte PMK */
	uint8_t key[64];
};

struct fw_link {
	uint8_t up;
	uint8_t ssid[32], ssid_len;	/* the network joined (also when wland joined it by itself: wlan.conf) */
	uint8_t bssid[6];
};

struct fw_bss {
	uint8_t bssid[6];
	uint8_t ssid[32], ssid_len;
	uint16_t frequency;		/* MHz */
	int16_t rssi;			/* dBm */
	uint16_t beacon_interval, capability;
	uint32_t age_ms;
	uint16_t elements_len;
	uint8_t elements[768];		/* the beacon's information elements */
};

#endif
