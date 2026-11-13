#ifndef _CC_KEYDERIVATION_H
#define _CC_KEYDERIVATION_H
#include <stddef.h>
#include <stdint.h>
enum { kCCPBKDF2 = 2 };
enum { kCCPRFHmacAlgSHA1 = 1, kCCPRFHmacAlgSHA256 = 2 };
int CCKeyDerivationPBKDF(int algorithm, const char *password, size_t passwordLen, const uint8_t *salt, size_t saltLen, int prf, unsigned rounds, uint8_t *derivedKey, size_t derivedKeyLen);
#endif
