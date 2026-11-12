/* OpenSSL-backed implementations of the host-test shims declared in host_stubs.h. */
#include "host_stubs.h"
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <string.h>

const uint8_t CCrfc3394_iv[8] = { 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6 };
const size_t CCrfc3394_ivLen = 8;

void
CCHmac(int algorithm, const void *key, size_t keyLength, const void *data, size_t dataLength, void *macOut)
{
	(void)algorithm;                       /* only SHA1 is used in radio.c */
	unsigned int len = 0;
	if (!HMAC(EVP_sha1(), key, (int)keyLength, data, dataLength, macOut, &len) || len != 20) {
		memset(macOut, 0, 20);
	}
}

int
CCKeyDerivationPBKDF(int algorithm, const char *password, size_t passwordLen,
                     const uint8_t *salt, size_t saltLen, int prf, unsigned rounds,
                     uint8_t *derivedKey, size_t derivedKeyLen)
{
	(void)algorithm;
	if (prf != kCCPRFHmacAlgSHA1) return -1;
	return PKCS5_PBKDF2_HMAC_SHA1(password, (int)passwordLen, salt, (int)saltLen,
	                              (int)rounds, (int)derivedKeyLen, derivedKey) == 1 ? 0 : -1;
}

/* RFC 3394 AES-KW unwrap, 128-bit KEK (the only use in radio.c):
 * for j=5..0, i=n..1: B = AES-1(K, (A^t)|R[i]); A = MSB(B); R[i] = LSB(B); t = n*j+i. */
int
CCSymmetricKeyUnwrap(int algorithm, const uint8_t *iv, size_t ivLen,
                     const uint8_t *key, size_t keyLen,
                     const uint8_t *wrapped, size_t wrappedLen,
                     uint8_t *out, size_t *outLen)
{
	(void)algorithm; (void)ivLen;
	if (keyLen != 16 || wrappedLen < 16 || wrappedLen % 8) return -1;
	uint8_t a[8], r[64], in[16], b[16];
	AES_KEY dk;
	if (AES_set_decrypt_key(key, 128, &dk) != 0) return -1;
	size_t n = wrappedLen / 8 - 1;
	if (n * 8 > sizeof(r)) return -1;
	memcpy(a, wrapped, 8);
	memcpy(r, wrapped + 8, n * 8);
	for (long j = 5; j >= 0; j--) {
		for (long i = n; i >= 1; i--) {
			uint64_t t = (uint64_t)n * (uint64_t)j + (uint64_t)i;
			for (int k = 0; k < 8; k++) in[k] = a[k] ^ (uint8_t)(t >> (56 - 8 * k));
			memcpy(in + 8, r + (i - 1) * 8, 8);
			AES_decrypt(in, b, &dk);
			memcpy(a, b, 8);
			memcpy(r + (i - 1) * 8, b + 8, 8);
		}
	}
	if (memcmp(a, iv, 8) != 0) return -1;
	memcpy(out, r, n * 8);
	*outLen = n * 8;
	return 0;
}


int
host_sysctlbyname(const char *name, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	if (strcmp(name, "kern.bootargs") == 0) {
		const char *args = getenv("WIFI_TEST_BOOTARGS");
		if (!args) args = "";
		size_t n = strlen(args) + 1;
		if (oldp && oldlenp && *oldlenp >= n) {
			memcpy(oldp, args, n);
			*oldlenp = n;
			return 0;
		}
		return -1;
	}
	(void)newp; (void)newlen;
	if (strcmp(name, "kern.boottime") == 0) {
		if (oldp && oldlenp && *oldlenp >= sizeof(struct sysctl_stub_boottime)) {
			struct sysctl_stub_boottime bt = { 0, 0 };
			memcpy(oldp, &bt, sizeof(bt));
			return 0;
		}
		return -1;
	}
	return -1;
}
