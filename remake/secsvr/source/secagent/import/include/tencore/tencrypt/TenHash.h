/* TenHash.h -- tencrypt_new 对外哈希 API（复刻）
   函数签名来自 DWARF（TenHash.cpp 桩）；返回值为推断（0=成功），
   待反汇编核对。二进制中符号为 C++ mangled（_Z6TenMd5PhjS_ 等），故不用 extern "C"。 */

#ifndef TENCRYPT_INCLUDE_TENHASH_H_
#define TENCRYPT_INCLUDE_TENHASH_H_

unsigned int TenMd5(unsigned char *pbyBuf, unsigned int dwSize, unsigned char *pbyResult);
unsigned int TenMd2(unsigned char *pbyBuf, unsigned int dwSize, unsigned char *pbyResult);
unsigned int TenMd4(unsigned char *pbyBuf, unsigned int dwSize, unsigned char *pbyResult);
unsigned int TenSha1(unsigned char *pbyBuf, unsigned int dwSize, unsigned char *pbyResult);
unsigned int TenHaval160(unsigned char *pbyData, unsigned int dwSize, unsigned char *pbyResult);
unsigned int TenHaval128(unsigned char *pbyData, unsigned int dwSize, unsigned char *pbyResult);
unsigned int TenRipeMd160(unsigned char *pbyData, unsigned int dwSize, unsigned char *pbyResult);
unsigned int TenRipeMd128(unsigned char *pbyData, unsigned int dwSize, unsigned char *pbyResult);
unsigned int TenGostHash(unsigned char *pbyData, unsigned int dwSize, unsigned char *pbyResult);
unsigned int TenCrc32(unsigned char *pbyBuf, unsigned int dwSize);
unsigned int TenAdler32(unsigned int dwAdler, unsigned char *pbyBuf, unsigned int dwSize);

#endif /* TENCRYPT_INCLUDE_TENHASH_H_ */
