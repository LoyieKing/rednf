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

#if defined(__ELF__) && defined(__i386__)
// 32 位基线布局校验（unsigned long == 4；ORIG 为 i386 ELF）
static_assert(sizeof(AES_ALG_INFO) == 0x40u + 60u * sizeof(unsigned long),
              "AES_ALG_INFO layout");
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
    unsigned long  total[4];    // +0x20/0x24/0x28/0x2c 消息长度（位计数；ORIG
                                // SHA256_Update 0x080be093 实证 buffered=(total[0]>>3)&0x3f，
                                // SHA256_Final 0x080be1fb 实证 buffer@+0x30 ⇒ total 占 4 槽）
    unsigned char  buffer[64];  // +0x30
};


struct SHA512_ALG_INFO   // Sha384/Sha512 共用内核（state 为 8×u64）
{
    unsigned long long state[8];    // +0x00
    unsigned long      total[4];    // +0x40/0x44/0x48/0x4c 消息长度（位计数；ORIG
                                    // SHA512_Update 0x080c28bf 实证 buffered=(total[0]>>3)&0x7f）
    unsigned char      buffer[128]; // +0x50
};

// ORIG SHA384 与 SHA512 布局相同（SHA384_Init @0x080c03a1 同构），
// 独立 struct 定义以匹配 ChecksumAlgo.h 的前置声明与 mangled。
struct SHA384_ALG_INFO
{
    unsigned long long state[8];    // +0x00
    unsigned long      total[4];    // +0x40/0x44/0x48/0x4c 位计数（同 SHA512）
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
// ---- DES / 3DES（ORIG 0x080abcbc..0x080b14a0 / 0x080addf8..0x080afa08）----
// 布局实证（DES_SetAlgInfo/DES_EncInit/DES3_EncInit/DES_KeySchedule 反汇编）：
//   +0x00 mode     1=ECB 2=CBC 3=OFB 4=CFB（同 AES）
//   +0x04 padding  1=NONE 2=PKCS7
//   +0x08 vec[8]    SetAlgInfo 拷入的初始向量备份（DES 块 8 字节）
//   +0x10 chain[8]  当前链值（EncInit 时由 vec 初始化）
//   +0x18 buf[8]    Update 阶段部分块缓存
//   +0x20 buffered  buf 中缓存字节数
//   +0x24 rk[]      16 轮 × 8 字节子密钥（每轮 6 字节有效 + 2 字节 0）；
//                   DES3 为 3 × 0x80 = 0x180 字节（K1 加密序 | K2 | K3）
struct DES_ALG_INFO
{
    unsigned long   mode;
    unsigned long   padding;
    unsigned char   vec[8];
    unsigned char   chain[8];
    unsigned char   buf[8];
    unsigned long   buffered;
    unsigned char   rk[0x80];
};

struct DES3_ALG_INFO
{
    unsigned long   mode;
    unsigned long   padding;
    unsigned char   vec[8];
    unsigned char   chain[8];
    unsigned char   buf[8];
    unsigned long   buffered;
    unsigned char   rk[0x180];
};

#if defined(__ELF__) && defined(__i386__)
static_assert(sizeof(DES_ALG_INFO) == 0x24u + 0x80u, "DES_ALG_INFO layout");
static_assert(sizeof(DES3_ALG_INFO) == 0x24u + 0x180u, "DES3_ALG_INFO layout");
#endif

// ORIG public DES 接口（签名与 mangled 逐字对应）
void DES_SetAlgInfo(unsigned long mode, unsigned long padding,
                    unsigned char* vec, DES_ALG_INFO* alg);                 // 0x080afaa4
int  DES_KeySchedule(unsigned char* key, unsigned long keyLen,
                     DES_ALG_INFO* alg);                                    // 0x080abcbc 返回错误码（DES3_KeySchedule 检查）
void DES_EncInit(DES_ALG_INFO* alg);                                        // 0x080afbf0
void DES_DecInit(DES_ALG_INFO* alg);                                        // 0x080b07ca
void DES_Encrypt(void* rkArea, unsigned char* block);                       // 0x080ac0ba 单块加密（rkArea=&alg->rk）
void DES_Decrypt(void* rkArea, unsigned char* block);                       // 0x080acf59 单块解密
int  DES_EncUpdate(DES_ALG_INFO* alg, unsigned char* in, unsigned long inLen,
                   unsigned char* out, unsigned long* outLen);              // 0x080b0393
int  DES_DecUpdate(DES_ALG_INFO* alg, unsigned char* in, unsigned long inLen,
                   unsigned char* out, unsigned long* outLen);              // 0x080b10a7
int  DES_EncFinal(DES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen); // 0x080b072f
int  DES_DecFinal(DES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen); // 0x080b14a0

// ORIG public DES 四模式解密 Final
int  ECB_DecFinal(DES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen); // 0x080b1187
int  CBC_DecFinal(DES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen); // 0x080b1255
int  OFB_DecFinal(DES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen); // 0x080b137d
int  CFB_DecFinal(DES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen); // 0x080b1403

// ORIG public 3DES 接口（0x080addf8..0x080afa08）
void DES3_SetAlgInfo(unsigned long mode, unsigned long padding,
                     unsigned char* vec, DES3_ALG_INFO* alg);               // 0x080ae00c
int  DES3_KeySchedule(unsigned char* key, unsigned long keyLen,
                      DES3_ALG_INFO* alg);                                  // 0x080addf8 返回错误码
void DES3_EncInit(DES3_ALG_INFO* alg);                                      // 0x080ae158
void DES3_DecInit(DES3_ALG_INFO* alg);                                      // 0x080aed32
void DES3_Encrypt(void* rkArea, unsigned char* block);                      // 0x080adf6c EDE
void DES3_Decrypt(void* rkArea, unsigned char* block);                      // 0x080adfbc
int  DES3_EncUpdate(DES3_ALG_INFO* alg, unsigned char* in, unsigned long inLen,
                    unsigned char* out, unsigned long* outLen);             // 0x080ae8fb
int  DES3_DecUpdate(DES3_ALG_INFO* alg, unsigned char* in, unsigned long inLen,
                    unsigned char* out, unsigned long* outLen);             // 0x080af60f
int  DES3_EncFinal(DES3_ALG_INFO* alg, unsigned char* out, unsigned long* outLen); // 0x080aec97
int  DES3_DecFinal(DES3_ALG_INFO* alg, unsigned char* out, unsigned long* outLen); // 0x080afa08

// ORIG public 3DES 四模式解密 Final
int  ECB_DecFinal(DES3_ALG_INFO* alg, unsigned char* out, unsigned long* outLen); // 0x080af6ef
int  CBC_DecFinal(DES3_ALG_INFO* alg, unsigned char* out, unsigned long* outLen); // 0x080af7bd
int  OFB_DecFinal(DES3_ALG_INFO* alg, unsigned char* out, unsigned long* outLen); // 0x080af8e5
int  CFB_DecFinal(DES3_ALG_INFO* alg, unsigned char* out, unsigned long* outLen); // 0x080af96b

// ---- CRijndael_fake（ORIG 0x080b824c..0x080b851c）----

// ---- CRijndael_fake（ORIG 0x080b824c..0x080b851c）----
// 布局：vptr@0x00、m_ctxObf@0x04（new 指针与 m_salt 异或混淆）、m_salt@0x08。
class CRijndael_fake
{
public:
    CRijndael_fake();
    virtual ~CRijndael_fake();

    int  Initialize(char const* key, int keyLen, char const* chain,
                    int chainLen, int blockSize, int mode, int padding);
    int  Encrypt(char const* in, char* out, unsigned int len);
    int  Decrypt(char const* in, char* out, unsigned int len);
    unsigned int Pad(char* buffer, int pos, unsigned int* pLen);
    int  GetBlockSize() const;

private:
    void*        m_ctxObf;   // +0x04
    unsigned int m_salt;     // +0x08
};

#endif  // DNF_GAME_CHECKSUMCRYPTO_H_
