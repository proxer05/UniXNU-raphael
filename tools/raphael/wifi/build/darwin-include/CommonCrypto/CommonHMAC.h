#ifndef _CC_HMAC_H
#define _CC_HMAC_H
#include <stddef.h>
/* Apple's CCHmac algorithm order (CommonHMAC.h) — NOT the PRF enum, which is
 * different (kCCPRFHmacAlgSHA1 = 1). Verified on the device: CCHmac(0) runs
 * HMAC-SHA1 and CCHmac(1) runs HMAC-MD5 (diagnostics/cchmac-test.c). With the
 * values swapped, every 4-way-handshake MIC came out as HMAC-MD5 and the AP
 * rejected message 2 forever ("incorrect password" in Settings). */
enum { kCCHmacAlgSHA1 = 0, kCCHmacAlgMD5 = 1, kCCHmacAlgSHA256 = 2 };
void CCHmac(int algorithm, const void *key, size_t keyLength, const void *data, size_t dataLength, void *macOut);
#endif
