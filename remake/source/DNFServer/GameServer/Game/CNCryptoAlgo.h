#ifndef GAME_CNCRYPTO_ALGO_H_
#define GAME_CNCRYPTO_ALGO_H_

// CNCrypto 包装层底层的标准分组密码实现（G7，2026-08-16）。
// 各 setup/ecb 函数移植自 LibTomCrypt（公有领域，SPDX: Unlicense），
// 与 ORIG 使用的 symmetric_key 上下文/错误码口径一致：
//   CRYPT_OK=0 / CRYPT_INVALID_KEYSIZE=3 / CRYPT_INVALID_ROUNDS=4。
// 算法正确即可（用户规则 2026-08-16），不要求与 ORIG 逐指令 identical。

#include <cstring>

typedef unsigned int   ulong32;
typedef unsigned long long ulong64;

// ---- LibTomCrypt 错误码（本批次用到部分）----
#define CRYPT_OK                0
#define CRYPT_ERROR             1
#define CRYPT_NOP               2
#define CRYPT_INVALID_KEYSIZE   3
#define CRYPT_INVALID_ROUNDS    4
#define CRYPT_FAIL_TESTVECTOR   5
#define CRYPT_BUFFER_OVERFLOW   6
#define CRYPT_INVALID_ARG       7

// ---- 宏兼容层 ----
#define LTC_ARGCHK(x)       ((void)0)
#define LTC_ARGCHKVD(x)     ((void)0)
#define LTC_UNUSED_PARAM(x) ((void)(x))
#define LTC_INLINE          inline
#define CONST64(n)          (n##ULL)
#define XMEMCPY             memcpy
#define XMEMSET             memset
#define XMEMCMP             memcmp
#define XSTRCMP             strcmp
#define burn_stack(x)       ((void)0)
#define zeromem(p, n)       memset((p), 0, (n))

#if defined(__i386__) || defined(__x86_64__)
#define ENDIAN_LITTLE
#endif

// ---- 端序宏（大端 LOAD/STORE 为算法规范要求，与宿主机端序无关）----
#define LOAD32H(x, y)                                    \
  do { (x) = ((ulong32)((y)[0] & 255) << 24) |           \
            ((ulong32)((y)[1] & 255) << 16) |            \
            ((ulong32)((y)[2] & 255) << 8)  |            \
            ((ulong32)((y)[3] & 255)); } while (0)

#define STORE32H(x, y)                                   \
  do { (y)[0] = (unsigned char)(((x) >> 24) & 255);      \
       (y)[1] = (unsigned char)(((x) >> 16) & 255);      \
       (y)[2] = (unsigned char)(((x) >> 8) & 255);       \
       (y)[3] = (unsigned char)((x) & 255); } while (0)

#define LOAD64H(x, y)                                     \
  do { (x) = ((ulong64)((y)[0] & 255) << 56) |            \
            ((ulong64)((y)[1] & 255) << 48) |             \
            ((ulong64)((y)[2] & 255) << 40) |             \
            ((ulong64)((y)[3] & 255) << 32) |             \
            ((ulong64)((y)[4] & 255) << 24) |             \
            ((ulong64)((y)[5] & 255) << 16) |             \
            ((ulong64)((y)[6] & 255) << 8)  |             \
            ((ulong64)((y)[7] & 255)); } while (0)

#define STORE64H(x, y)                                    \
  do { (y)[0] = (unsigned char)(((x) >> 56) & 255);       \
       (y)[1] = (unsigned char)(((x) >> 48) & 255);       \
       (y)[2] = (unsigned char)(((x) >> 40) & 255);       \
       (y)[3] = (unsigned char)(((x) >> 32) & 255);       \
       (y)[4] = (unsigned char)(((x) >> 24) & 255);       \
       (y)[5] = (unsigned char)(((x) >> 16) & 255);       \
       (y)[6] = (unsigned char)(((x) >> 8) & 255);        \
       (y)[7] = (unsigned char)((x) & 255); } while (0)

#define LOAD32L(x, y)                                     \
  do { (x) = ((ulong32)((y)[3] & 255) << 24) |            \
            ((ulong32)((y)[2] & 255) << 16) |             \
            ((ulong32)((y)[1] & 255) << 8)  |             \
            ((ulong32)((y)[0] & 255)); } while (0)

#define STORE32L(x, y)                                    \
  do { (y)[3] = (unsigned char)(((x) >> 24) & 255);       \
       (y)[2] = (unsigned char)(((x) >> 16) & 255);       \
       (y)[1] = (unsigned char)(((x) >> 8) & 255);        \
       (y)[0] = (unsigned char)((x) & 255); } while (0)

#define ROLc(x, y)  ((((ulong32)(x) << (y)) | ((ulong32)(x) >> (32 - (y)))) & 0xFFFFFFFFUL)
#define RORc(x, y)  ((((ulong32)(x) >> (y)) | ((ulong32)(x) << (32 - (y)))) & 0xFFFFFFFFUL)
#define ROL(x, y)   ROLc(x, y)
#define ROR(x, y)   RORc(x, y)
#define LTC_BYTE(x, n) (((x) >> (8 * (n))) & 255)
#define LTC_ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#ifndef LTC_FALLTHROUGH
#define LTC_FALLTHROUGH ((void)0)
#endif

// ============================================================================
// symmetric_key：各算法密钥上下文联合（字段与 LibTomCrypt 一致）
// ============================================================================

#define LTC_ANUBIS
#define LTC_BLOWFISH
#define LTC_CAST5
#define LTC_KASUMI
#define LTC_KHAZAD
#define LTC_MULTI2
#define LTC_NOEKEON
#define LTC_SKIPJACK
#define LTC_TWOFISH
#define LTC_XTEA

struct symmetric_key
{
    union
    {
        struct {
            ulong32 roundKeyEnc[18 + 1][4];
            ulong32 roundKeyDec[18 + 1][4];
            int keyBits;
            int R;
        } anubis;
        struct {
            ulong32 S[4][256];
            ulong32 K[18];
        } blowfish;
        struct {
            ulong32 K[32], keylen;
        } cast5;
        struct {
            ulong32 KLi1[8], KLi2[8], KOi1[8], KOi2[8], KOi3[8],
                    KIi1[8], KIi2[8], KIi3[8];
        } kasumi;
        struct {
            ulong64 roundKeyEnc[8 + 1];
            ulong64 roundKeyDec[8 + 1];
        } khazad;
        struct {
            ulong32 uk[8];
            int N;
        } multi2;
        struct {
            ulong32 K[4], dK[4];
        } noekeon;
        struct {
            unsigned char key[10];
        } skipjack;
        struct {
            ulong32 S[4][256], K[40];
        } twofish;
        struct {
            ulong32 A[32], B[32];
        } xtea;
    };
};

// ---- 各算法公开接口 ----
int  anubis_setup(const unsigned char* key, int keylen, int num_rounds, symmetric_key* skey);
int  anubis_ecb_encrypt(const unsigned char* pt, unsigned char* ct, const symmetric_key* skey);
int  anubis_ecb_decrypt(const unsigned char* ct, unsigned char* pt, const symmetric_key* skey);

int  blowfish_setup(const unsigned char* key, int keylen, int num_rounds, symmetric_key* skey);
int  blowfish_setup_with_data(const unsigned char* key, int keylen,
                              const unsigned char* data, int datalen, symmetric_key* skey);
int  blowfish_ecb_encrypt(const unsigned char* pt, unsigned char* ct, const symmetric_key* skey);
int  blowfish_ecb_decrypt(const unsigned char* ct, unsigned char* pt, const symmetric_key* skey);

int  cast5_setup(const unsigned char* key, int keylen, int num_rounds, symmetric_key* skey);
int  cast5_ecb_encrypt(const unsigned char* pt, unsigned char* ct, const symmetric_key* skey);
int  cast5_ecb_decrypt(const unsigned char* ct, unsigned char* pt, const symmetric_key* skey);

int  kasumi_setup(const unsigned char* key, int keylen, int num_rounds, symmetric_key* skey);
int  kasumi_ecb_encrypt(const unsigned char* pt, unsigned char* ct, const symmetric_key* skey);
int  kasumi_ecb_decrypt(const unsigned char* ct, unsigned char* pt, const symmetric_key* skey);

int  khazad_setup(const unsigned char* key, int keylen, int num_rounds, symmetric_key* skey);
int  khazad_ecb_encrypt(const unsigned char* pt, unsigned char* ct, const symmetric_key* skey);
int  khazad_ecb_decrypt(const unsigned char* ct, unsigned char* pt, const symmetric_key* skey);

int  multi2_setup(const unsigned char* key, int keylen, int num_rounds, symmetric_key* skey);
int  multi2_ecb_encrypt(const unsigned char* pt, unsigned char* ct, const symmetric_key* skey);
int  multi2_ecb_decrypt(const unsigned char* ct, unsigned char* pt, const symmetric_key* skey);

int  noekeon_setup(const unsigned char* key, int keylen, int num_rounds, symmetric_key* skey);
int  noekeon_ecb_encrypt(const unsigned char* pt, unsigned char* ct, const symmetric_key* skey);
int  noekeon_ecb_decrypt(const unsigned char* ct, unsigned char* pt, const symmetric_key* skey);

int  skipjack_setup(const unsigned char* key, int keylen, int num_rounds, symmetric_key* skey);
int  skipjack_ecb_encrypt(const unsigned char* pt, unsigned char* ct, const symmetric_key* skey);
int  skipjack_ecb_decrypt(const unsigned char* ct, unsigned char* pt, const symmetric_key* skey);

int  twofish_setup(const unsigned char* key, int keylen, int num_rounds, symmetric_key* skey);
int  twofish_ecb_encrypt(const unsigned char* pt, unsigned char* ct, const symmetric_key* skey);
int  twofish_ecb_decrypt(const unsigned char* ct, unsigned char* pt, const symmetric_key* skey);

int  xtea_setup(const unsigned char* key, int keylen, int num_rounds, symmetric_key* skey);
int  xtea_ecb_encrypt(const unsigned char* pt, unsigned char* ct, const symmetric_key* skey);
int  xtea_ecb_decrypt(const unsigned char* ct, unsigned char* pt, const symmetric_key* skey);

// RC6（ORIG 用 RC6_ALG_INFO 上下文，接口保留；算法为标准 RC6）
struct RC6_ALG_INFO
{
    unsigned char key[0x20];  // 原始密钥（rc6_key_setup 取前 keylen 字节）
    unsigned int K[44];       // RC6 轮密钥（w=32, r=20）
};

void rc6_key_setup(RC6_ALG_INFO* alg, int keylen);
void rc6_block_encrypt(const RC6_ALG_INFO* alg, const unsigned int* in, unsigned int* out);
void rc6_block_decrypt(const RC6_ALG_INFO* alg, const unsigned int* in, unsigned int* out);

#endif  // GAME_CNCRYPTO_ALGO_H_
