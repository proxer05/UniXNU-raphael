/* Host-test shims for the vendored iOS sources. NOT built for the phone.
 * CommonCrypto's three uses in radio.c (CCHmac-SHA1, PBKDF2-SHA1, AES key wrap
 * unwrap) backed by OpenSSL; sysctl reads backed by getenv/fakes. */
#ifndef WIFI_HOST_STUBS_H
#define WIFI_HOST_STUBS_H
#include <stddef.h>
#include <stdint.h>

/* --- CommonCrypto subset used by src/radio.c --- */
enum { kCCHmacAlgSHA1 = 1, kCCPRFHmacAlgSHA1 = 1, kCCPBKDF2 = 2, kCCWRAPAES = 1 };
extern const uint8_t CCrfc3394_iv[8];
extern const size_t CCrfc3394_ivLen;

void CCHmac(int algorithm, const void *key, size_t keyLength, const void *data, size_t dataLength, void *macOut);
int CCKeyDerivationPBKDF(int algorithm, const char *password, size_t passwordLen,
                         const uint8_t *salt, size_t saltLen, int prf, unsigned rounds,
                         uint8_t *derivedKey, size_t derivedKeyLen);
int CCSymmetricKeyUnwrap(int algorithm, const uint8_t *iv, size_t ivLen,
                         const uint8_t *key, size_t keyLen,
                         const uint8_t *wrapped, size_t wrappedLen,
                         uint8_t *out, size_t *outLen);

/* --- sysctl subset used by bridge.c / rmtfs.c / wland.c (oldp=NULL/oldlenp=NULL
 * for the newp arguments is all the callers rely on) --- */
struct sysctl_stub_boottime { long sec, usec; };
int host_sysctlbyname(const char *name, void *oldp, size_t *oldlenp, void *newp, size_t newlen);

#endif
