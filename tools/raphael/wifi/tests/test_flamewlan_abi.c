/* /dev/wlan message ABI (src/flamewlan.h): packing, framing and round-trip checks.
 * The kernel side of the port must use this exact header, so any layout change is a
 * compatibility break this test has to catch. */
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "flamewlan.h"

static int failures;

static void
check(int ok, const char *what)
{
	printf("%s %s\n", ok ? "ok  " : "FAIL", what);
	if (!ok) failures++;
}

int
main(void)
{
	check(sizeof(struct fw_hdr) == 4, "fw_hdr is 4 bytes");
	check(sizeof(struct fw_hello) == 6, "fw_hello is 6 bytes");
	check(sizeof(struct fw_join) == 106, "fw_join is 106 bytes");
	check(offsetof(struct fw_join, ssid) == 0 && offsetof(struct fw_join, bssid) == 33
	      && offsetof(struct fw_join, key_kind) == 40 && offsetof(struct fw_join, key) == 42,
	      "fw_join field order");
	check(sizeof(struct fw_link) == 40, "fw_link is 40 bytes");
	check(sizeof(struct fw_bss) == 824, "fw_bss is 824 bytes");
	check(offsetof(struct fw_bss, elements_len) == 52 && offsetof(struct fw_bss, elements) == 54,
	      "fw_bss field order");
	check(FW_FRAME_MAX == 1514 && FW_MSG_MAX == sizeof(struct fw_hdr) + 1600, "frame/message limits");

	/* a message frames exactly: header then len payload bytes */
	uint8_t buf[FW_MSG_MAX];
	struct fw_hdr h = { .type = FW_JOIN, .len = sizeof(struct fw_join) };
	struct fw_join j;
	memset(&j, 0, sizeof(j));
	memcpy(j.ssid, "raphael", 7);
	j.ssid_len = 7;
	memcpy(buf, &h, sizeof(h));
	memcpy(buf + sizeof(h), &j, sizeof(j));
	struct fw_hdr *rh = (struct fw_hdr *)buf;
	check(rh->type == FW_JOIN && rh->len == sizeof(struct fw_join), "join header round-trips");
	struct fw_join *rj = (struct fw_join *)(buf + sizeof(*rh));
	check(rj->ssid_len == 7 && memcmp(rj->ssid, "raphael", 7) == 0, "join payload round-trips");

	/* a frame message: header then an Ethernet frame */
	h.type = FW_TX; h.len = 60;
	uint8_t frame[60];
	memset(frame, 0xaa, sizeof(frame));
	memcpy(buf, &h, sizeof(h));
	memcpy(buf + sizeof(h), frame, sizeof(frame));
	rh = (struct fw_hdr *)buf;
	check(rh->type == FW_TX && rh->len == 60, "tx frame round-trips");

	return failures ? 1 : 0;
}
