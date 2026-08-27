#ifndef DNF_GAME_CHECKSUMCRYPTO_H_
#define DNF_GAME_CHECKSUMCRYPTO_H_

// Wave9_Crypto —— 校验/AES 密码学族权威声明。
// ORIG 证据（df_game_r nm -S / objdump / docs/class_func_reports）：
//   IChecksum 族        0x0809de70..0x080992f0   （CNChecksum{Md5,Sha256,Sha384,Sha512}）
//   SHA*_ALG_INFO 族    _Z11SHA256_Init... @0x080be020 / 0x080c03a1 / 0x080c27f1
//   AES 族              _Z14AES_SetAlgInfo @0x080a267c 等 11 个 public 符号
//                       （RIJNDAEL_KeySchedule/PaddSet/PaddCheck/模式 Update/Final 为 file-static，
//                        其中 *_DecFinal 四个为 public T @0x080a4396..0x080a47aa）
//   CRijndael_fake      0x080b824c..0x080b851c
// 实现见 ChecksumCrypto.cpp（单 TU；算法为语义正确真实现，不逐指令 identical）。

#include "ChecksumAlgo.h"

// ---- AES/Rijndael 分派错误码（ORIG 立即数）----
static const int AES_ERR_BAD_MODE       = (int)0x00001001;
static const int AES_ERR_BAD_KEYLEN     = (int)0x00001002;
static const int AES_ERR_PAD_CHECK      = (int)0x00001003;
static const int AES_ERR_PAD_OVERFLOW   = (int)0x00001004;

// ORIG AES_ALG_INFO 布局（AES_SetAlgInfo/EncInit/*Update 反汇编实证）：
//   +0x00 mode     1=ECB 2=CBC 3=OFB 4=CFB
//   +0x04 padding  1=NONE 2=PKCS7
//   +0x08 vec[16]  SetAlgInfo 拷入的初始向量备份
//   +0x18 chain[16] 当前链值（EncInit 时由 vec 初始化）
//   +0x28 buf[16]  Update 阶段部分块缓存
//   +0x38 buffered buf 中缓存字节数
//   +0x3c num      Nk=(keyLen+3)/4（密钥字数）
//   +0x40 rk[60]   扩展轮密钥（enc/dec 各自 schedule 后占用）
struct AES_ALG_INFO
{
    unsigned long   mode;
    unsigned long   padding;
    unsigned char   vec[0x10];
    unsigned char   chain[0x10];
    unsigned char   buf[0x10];
    unsigned long   buffered;
    unsigned long   num;
    unsigned long   rk[60];
};

#if defined(__GNUC__)
// 32 位基线布局校验（unsigned long == 4）
static_assert(sizeof(AES_ALG_INFO) == 0x40 + 240, "AES_ALG_INFO layout");
#endif

// ORIG public AES 接口（签名与 mangled 逐字对应）
void AES_SetAlgInfo(unsigned long mode, unsigned long padding,
                    unsigned char* vec, AES_ALG_INFO* alg);                 // 0x080a267c
void AES_EncKeySchedule(unsigned char* key, unsigned long keyLen,
                        AES_ALG_INFO* alg);                                 // 0x0809f324
void AES_DecKeySchedule(unsigned char* key, unsigned long keyLen,
                        AES_ALG_INFO* alg);                                 // 0x0809f389
void AES_EncInit(AES_ALG_INFO* alg);                                        // 0x080a27c8
void AES_DecInit(AES_ALG_INFO* alg);                                        // 0x080a36c8
int  AES_EncUpdate(AES_ALG_INFO* alg, unsigned char* in, unsigned long inLen,
                   unsigned char* out, unsigned long* outLen);              // 0x080a31bc
int  AES_DecUpdate(AES_ALG_INFO* alg, unsigned char* in, unsigned long inLen,
                   unsigned char* out, unsigned long* outLen);              // 0x080a42b6
int  AES_EncFinal(AES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen); // 0x080a362d
int  AES_DecFinal(AES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen); // 0x080a47aa
void AES_Encrypt(void* pNumArea, unsigned char* block);                     // 0x0809f981 单块加密（pNumArea=&alg->num, ek=num+4）
void AES_Decrypt(void* pNumArea, unsigned char* block);                     // 0x080a0ffe 单块解密

// ORIG 中暴露为 public 的四种解密 Final（模式分派前的内联版本亦被调用方直连使用）
int  ECB_DecFinal(AES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen); // 0x080a4396
int  CBC_DecFinal(AES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen); // 0x080a448d
int  OFB_DecFinal(AES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen); // 0x080a4641
int  CFB_DecFinal(AES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen); // 0x080a46c7

// ---- SHA256/384/512_ALG_INFO（ORIG Init 反汇编实证字段序）----
struct SHA256_ALG_INFO
{
    unsigned int   state[8];    // +0x00（0x6a09e667... 0x5be0cd19）
    unsigned int   total[2];    // +0x20 lo/+0x24 hi 已处理字节数
    unsigned char  buffer[64];  // +0x28
};
struct SHA512_ALG_INFO   // 供 Sha384/Sha512 共用内核（state 为 8×u64，大端语义）
{
    unsigned long long state[8];    // +0x00
    unsigned long long total[2];    // +0x40 lo/+0x48 hi
    unsigned char      buffer[128]; // +0x50
};

void SHA256_Init(SHA256_ALG_INFO* ctx);                                     // 0x080be020
void SHA256_Update(SHA256_ALG_INFO* ctx, unsigned char* in,
                   unsigned long len);                                      // 0x080be08a
void SHA256_Final(SHA256_ALG_INFO* ctx, unsigned char* out);                // 0x080be1d1
void SHA384_Init(SHA384_ALG_INFO* ctx);                                     // 0x080c03a1
void SHA384_Update(SHA384_ALG_INFO* ctx, unsigned char* in,
                   unsigned long len);                                      // 0x080c0469
void SHA384_Final(SHA384_ALG_INFO* ctx, unsigned char* out);                // 0x080c062e
void SHA512_Init(SHA512_ALG_INFO* ctx);                                     // 0x080c27f1
void SHA512_Update(SHA512_ALG_INFO* ctx, unsigned char* in,
                   unsigned long len);                                      // 0x080c28b9
void SHA512_Final(SHA512_ALG_INFO* ctx, unsigned char* out);                // 0x080c2a7e

#endif  // DNF_GAME_CHECKSUMCRYPTO_H_
