#!/bin/sh
# Cross-build wland for iOS 18 without an Apple SDK: minimal self-authored Darwin
# headers (build/darwin-include) + .tbd stubs generated from our own dyld cache
# (build/ios-sysroot) + the Rust lld wrapper + the pinned ldid for ad-hoc signing.
set -eu
cd "$(dirname "$0")"
CC=${CC:-clang}
LD=/hdd/raphael-images/ios18-gpu-setup-YJykZV4H/host-tools/ld64.lld
LDID=/hdd/UniXNU-raphael/research/jb/ldid-host-v2.1.5-procursus7/ldid_linux_x86_64
OUT=build
INC=-I$OUT/darwin-include
CFLAGS="-target arm64-apple-ios18.0 -O2 -Wall -Wextra -Wno-unused-parameter -fno-stack-protector -nostdinc $INC"
mkdir -p "$OUT/wland"
SRCS="wland smem pmem glink link qrtr_mux qmi rmtfs pdmapper servreg tftp scm wlfw radio radio_dma ipnet bridge"
for f in $SRCS; do
	$CC $CFLAGS -c "src/$f.c" -o "$OUT/wland/$f.o"
done
$LD -arch arm64 -platform_version ios 18.0 18.0 \
	-o "$OUT/wland/wland" \
	$(for f in $SRCS; do echo "$OUT/wland/$f.o"; done) \
	$OUT/ios-sysroot/usr/lib/system/libsystem_c.tbd \
	$OUT/ios-sysroot/usr/lib/system/libsystem_kernel.tbd \
	$OUT/ios-sysroot/usr/lib/system/libsystem_malloc.tbd \
	$OUT/ios-sysroot/usr/lib/system/libsystem_platform.tbd \
	$OUT/ios-sysroot/usr/lib/system/libsystem_pthread.tbd \
	$OUT/ios-sysroot/usr/lib/system/libsystem_m.tbd \
	$OUT/ios-sysroot/usr/lib/system/libcommonCrypto.tbd \
	$OUT/ios-sysroot/usr/lib/system/libsystem_info.tbd \
	$OUT/ios-sysroot/usr/lib/system/libremovefile.tbd \
	$OUT/ios-sysroot/usr/lib/system/libsystem_asl.tbd
"$LDID" -S "$OUT/wland/wland"
file "$OUT/wland/wland" || true
sha256sum "$OUT/wland/wland"
