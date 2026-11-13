#ifndef _CC_KEYWRAP_H
#define _CC_KEYWRAP_H
#include <stddef.h>
enum { kCCWRAPAES = 1 };
extern const unsigned char CCrfc3394_iv[8];
extern const size_t CCrfc3394_ivLen;
int CCSymmetricKeyWrap(int algorithm, const unsigned char *iv, size_t ivLen, const unsigned char *key, size_t keyLen, const unsigned char *rawKey, size_t rawKeyLen, unsigned char *wrappedKey, size_t *wrappedKeyLen);
int CCSymmetricKeyUnwrap(int algorithm, const unsigned char *iv, size_t ivLen, const unsigned char *key, size_t keyLen, const unsigned char *wrappedKey, size_t wrappedKeyLen, unsigned char *rawKey, size_t *rawKeyLen);
#endif
