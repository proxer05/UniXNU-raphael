#!/bin/sh
# Host build + tests for the vendored Wi-Fi sources (Linux, cc + OpenSSL).
#   tests/run.sh          compile the protocol subset and run the unit tests
# Nothing here is built for or run on the phone.
set -eu
cd "$(dirname "$0")"
CC=${CC:-cc}
CFLAGS="-std=c11 -Wall -Wextra -Wno-unused-parameter -O1 -g -I../src -Ifakeinc -D_GNU_SOURCE -DEAUTH=80"  # EAUTH: Darwin errno, absent from glibc
OUT=$(mktemp -d)
trap 'rm -rf "$OUT"' EXIT

# The donor's own build list (phone/wifi/build.sh) minus the device-facing mains
# (utun, /dev/wlan, the wland daemon loop), which need a phone. ath.c/ath.h are not
# in the donor's build list (incomplete leftovers) and are not vendored.
SRCS="../src/smem.c ../src/pmem.c ../src/glink.c ../src/link.c ../src/qrtr_mux.c \
      ../src/qmi.c ../src/rmtfs.c ../src/pdmapper.c ../src/servreg.c ../src/tftp.c \
      ../src/scm.c ../src/wlfw.c ../src/radio.c"

echo "== compiling $(printf '%s\n' $SRCS | wc -l) protocol sources =="
# Host transform (src/ stays verbatim): the ARM64 "dsb sy" barrier is not
# assemblable on x86; on the host it is a no-op comment.
for f in $SRCS; do
	cp "$f" "$OUT/$(basename "$f")"
	sed -i 's|__asm__ volatile("dsb sy" ::: "memory");|(void)0; /* host: dsb */|' "$OUT/$(basename "$f")"
	$CC $CFLAGS -Dsysctlbyname=host_sysctlbyname -c "$OUT/$(basename "$f")" -o "$OUT/$(basename "$f" .c).o"
done

echo "== stub self-tests (RFC 6070 PBKDF2, RFC 3394 unwrap, HMAC-SHA1 RFC 2202) =="
$CC $CFLAGS -Dsysctlbyname=host_sysctlbyname test_crypto_stubs.c host_stubs.c -o "$OUT/t_crypto" -lcrypto
"$OUT/t_crypto"

echo "== /dev/wlan message ABI tests =="
$CC $CFLAGS test_flamewlan_abi.c -o "$OUT/t_abi"
"$OUT/t_abi"

echo 'run.sh: OK'
