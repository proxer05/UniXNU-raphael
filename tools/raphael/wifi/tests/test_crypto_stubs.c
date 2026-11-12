/* Validate the OpenSSL-backed CommonCrypto shims against published test vectors
 * (RFC 2202 HMAC-SHA1, RFC 6070 PBKDF2-SHA1, RFC 3394 AES-KW unwrap). */
#include <stdio.h>
#include <string.h>
#include "host_stubs.h"

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
	/* RFC 2202 test case 2 */
	uint8_t mac[20];
	const char *key = "Jefe";
	const char *data = "what do ya want for nothing?";
	uint8_t want_mac[20] = { 0xef,0xfc,0xdf,0x6a,0xe5,0xeb,0x2f,0xa2,0xd2,0x74,
	                         0x16,0xd5,0xf1,0x84,0xdf,0x9c,0x25,0x9a,0x7c,0x79 };
	CCHmac(kCCHmacAlgSHA1, key, strlen(key), data, strlen(data), mac);
	check(memcmp(mac, want_mac, 20) == 0, "CCHmac SHA1 (RFC 2202 #2)");

	/* RFC 6070 test case 1: "password"/"salt", 1 round, SHA1->20 bytes */
	uint8_t dk[20];
	uint8_t want_dk[20] = { 0x0c,0x60,0xc8,0x0f,0x96,0x1f,0x0e,0x71,0xf3,0xa9,
	                        0xb5,0x24,0xaf,0x60,0x12,0x06,0x2f,0xe0,0x37,0xa6 };
	check(CCKeyDerivationPBKDF(kCCPBKDF2, "password", 8, (const uint8_t *)"salt", 4,
	      kCCPRFHmacAlgSHA1, 1, dk, 20) == 0 && memcmp(dk, want_dk, 20) == 0,
	      "PBKDF2-SHA1 (RFC 6070 #1)");

	/* RFC 3394 4.1: KEK 000102...0f, wrap 00112233445566778899aabbccddeeff -> unwrap back */
	uint8_t kek[16] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };
	uint8_t wrapped[24] = { 0x1f,0xa6,0x8b,0x0a,0x81,0x12,0xb4,0x47,
	                        0xae,0xf3,0x4b,0xd8,0xfb,0x5a,0x7b,0x82,
	                        0x9d,0x3e,0x86,0x23,0x71,0xd2,0xcf,0xe5 };
	uint8_t plain[16];
	size_t plen = 0;
	check(CCSymmetricKeyUnwrap(kCCWRAPAES, CCrfc3394_iv, CCrfc3394_ivLen, kek, 16,
	      wrapped, sizeof(wrapped), plain, &plen) == 0 && plen == 16 && plain[0] == 0x00
	      && plain[15] == 0xff, "AES-KW unwrap (RFC 3394 4.1)");

	return failures ? 1 : 0;
}
