/* TenHash.cpp -- tencrypt_new 对外哈希 API（复刻）
   签名/局部变量来自 DWARF 桩（TenHash.cpp）。
   返回值语义（0=成功）为推断，待反汇编逐字节核对。
   TODO(haval/ripemd/gost)：CHaval128/160、CRipeMD128/160、CGostHash 实现就绪后补全
   TenHaval160/TenHaval128/TenRipeMd160/TenRipeMd128/TenGostHash。 */

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
    memcpy(pbyResult, digest, 16);
    return 0;
}

unsigned int TenMd2(unsigned char *pbyBuf, unsigned int dwSize, unsigned char *pbyResult) {
    md2_t ctx;
    CMd2 objMd2;
    objMd2.md2_init(&ctx);
    objMd2.md2_update_data(&ctx, pbyBuf, dwSize);
    objMd2.md2_finish(&ctx, pbyResult, 16);
    return 0;
}

unsigned int TenMd4(unsigned char *pbyBuf, unsigned int dwSize, unsigned char *pbyResult) {
    MD4_CTX ctx;
    CMD4 objMD4;
    unsigned char digest[16];
    objMD4.MD4Init(&ctx);
    objMD4.MD4Update(&ctx, pbyBuf, dwSize);
    objMD4.MD4Final(digest, &ctx);
    memcpy(pbyResult, digest, 16);
    return 0;
}

unsigned int TenSha1(unsigned char *pbyBuf, unsigned int dwSize, unsigned char *pbyResult) {
    SHA1_CTX ctx;
    CSha1 objSha1;
    objSha1.SHA1_Init(&ctx);
    objSha1.SHA1_Update(&ctx, pbyBuf, dwSize);
    objSha1.SHA1_Final(pbyResult, &ctx);
    return 0;
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
    objMd160.AddData(pbyData, dwSize);
    objMd160.FinalDigest(pbyResult);
    return 0;
}

unsigned int TenRipeMd128(unsigned char *pbyData, unsigned int dwSize, unsigned char *pbyResult) {
    CRipeMD128 objMd128;
    objMd128.AddData(pbyData, dwSize);
    objMd128.FinalDigest(pbyResult);
    return 0;
}

unsigned int TenHaval160(unsigned char *pbyData, unsigned int dwSize, unsigned char *pbyResult) {
    CHaval160 objHaval160;
    objHaval160.havalUpdate(pbyData, dwSize);
    objHaval160.havalFinal(pbyResult);
    return 0;
}

unsigned int TenHaval128(unsigned char *pbyData, unsigned int dwSize, unsigned char *pbyResult) {
    CHaval128 objHaval128;
    objHaval128.havalUpdate(pbyData, dwSize);
    objHaval128.havalFinal(pbyResult);
    return 0;
}

unsigned int TenGostHash(unsigned char *pbyData, unsigned int dwSize, unsigned char *pbyResult) {
    CGostHash objGost;
    objGost.gosthash_update(pbyData, dwSize);
    objGost.gosthash_final(pbyResult);
    return 0;
}
