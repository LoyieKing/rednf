/* TenHash.cpp -- tencrypt_new 对外哈希 API（复刻）
   签名/局部变量来自 DWARF 桩（TenHash.cpp）。
   返回值 = 摘要字节数（ORIG 反汇编：MD5/MD2/MD4/HAVAL128/RIPEMD128=0x10，
   SHA1/HAVAL160/RIPEMD160=0x14，GOST=0x20；Crc32/Adler32 返回算法值）。
   调用序列按 ORIG 反汇编核对：HAVAL 显式 havalInit、RIPEMD 显式 Reset、
   GOST 显式 gosthash_reset；MD2 的 md2_finish 第 3 参为 dwSize（非固定 16）。 */

#include "include/TenHash.h"
#include "inc/md5.h"
#include "inc/sha1.h"
#include "inc/md2.h"
#include "inc/md4.h"
#include "inc/crc32.h"
#include "inc/adler32.h"
#include "inc/ripemd160.h"
#include "inc/ripemd128.h"
#include "inc/haval160.h"
#include "inc/haval128.h"
#include "inc/gosthash.h"

#include <string.h>

unsigned int TenMd5(unsigned char *pbyBuf, unsigned int dwSize, unsigned char *pbyResult) {
    md5_state_t state;
    md5_byte_t digest[16];
    md5_init(&state);
    md5_append(&state, pbyBuf, (int)dwSize);
    md5_finish(&state, digest);
    memmove(pbyResult, digest, 16);
    return 16;
}

unsigned int TenMd2(unsigned char *pbyBuf, unsigned int dwSize, unsigned char *pbyResult) {
    md2_t ctx;
    CMd2 objMd2;
    objMd2.md2_init(&ctx);
    objMd2.md2_update_data(&ctx, pbyBuf, dwSize);
    objMd2.md2_finish(&ctx, pbyResult, dwSize);
    return 16;
}

unsigned int TenMd4(unsigned char *pbyBuf, unsigned int dwSize, unsigned char *pbyResult) {
    MD4_CTX ctx;
    CMD4 objMD4;
    unsigned char digest[16];
    objMD4.MD4Init(&ctx);
    objMD4.MD4Update(&ctx, pbyBuf, dwSize);
    objMD4.MD4Final(digest, &ctx);
    memmove(pbyResult, digest, 16);
    return 16;
}

unsigned int TenSha1(unsigned char *pbyBuf, unsigned int dwSize, unsigned char *pbyResult) {
    SHA1_CTX ctx;
    CSha1 objSha1;
    objSha1.SHA1_Init(&ctx);
    objSha1.SHA1_Update(&ctx, pbyBuf, dwSize);
    objSha1.SHA1_Final(pbyResult, &ctx);
    return 20;
}

unsigned int TenCrc32(unsigned char *pbyBuf, unsigned int dwSize) {
    CCrc32 objCrc32;
    return objCrc32.Crc32(pbyBuf, dwSize);
}

unsigned int TenAdler32(unsigned int dwAdler, unsigned char *pbyBuf, unsigned int dwSize) {
    CAdler32 objAdler32;
    return objAdler32.adler32(dwAdler, pbyBuf, dwSize);
}

unsigned int TenRipeMd160(unsigned char *pbyData, unsigned int dwSize, unsigned char *pbyResult) {
    CRipeMD160 objMd160;
    objMd160.Reset();
    objMd160.AddData(pbyData, dwSize);
    objMd160.FinalDigest(pbyResult);
    return 20;
}

unsigned int TenRipeMd128(unsigned char *pbyData, unsigned int dwSize, unsigned char *pbyResult) {
    CRipeMD128 objMd128;
    objMd128.Reset();
    objMd128.AddData(pbyData, dwSize);
    objMd128.FinalDigest(pbyResult);
    return 16;
}

unsigned int TenHaval160(unsigned char *pbyData, unsigned int dwSize, unsigned char *pbyResult) {
    CHaval160 objHaval160;
    objHaval160.havalInit();
    objHaval160.havalUpdate(pbyData, dwSize);
    objHaval160.havalFinal(pbyResult);
    return 20;
}

unsigned int TenHaval128(unsigned char *pbyData, unsigned int dwSize, unsigned char *pbyResult) {
    CHaval128 objHaval128;
    objHaval128.havalInit();
    objHaval128.havalUpdate(pbyData, dwSize);
    objHaval128.havalFinal(pbyResult);
    return 16;
}

unsigned int TenGostHash(unsigned char *pbyData, unsigned int dwSize, unsigned char *pbyResult) {
    CGostHash objGost;
    objGost.gosthash_reset();
    objGost.gosthash_update(pbyData, dwSize);
    objGost.gosthash_final(pbyResult);
    return 32;
}
