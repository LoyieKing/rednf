// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CNCrypto/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r CNCrypto* 统一分组密码封装（G7，2026-08-16 还原）。
// 接口/错误码与 docs/class_func_reports/CNCrypto*.md 一致；底层算法见
// CNCryptoAlgo.cpp（标准实现，算法正确即可）。

#include "CNCrypto.h"

#include <cstring>

#include "CRijndael.h"
#include "CEnvironment.h"   // CTEA
#include "CNCryptoAlgo.h"
#include "CBlowFish.h"

// ORIG TU-local static（nm：t _ZL3ROLji @0x0809c418 / t _ZL3RORji @0x0809c42d，
// 体 `x = ROLc/RORc(x,y); return x;`，4.1 发寄存器 rol/ror + 写回形参槽）。
#undef ROL
#undef ROR
static unsigned int ROL(unsigned int x, int y)
{
    x = ROLc(x, y);
    return x;
}

static unsigned int ROR(unsigned int x, int y)
{
    x = RORc(x, y);
    return x;
}

// ============================================================================
// CNCryptoShift（SHIFT：32 位 ROR + XOR，块 4 / 钥 8）
// ============================================================================

#include "CNCrypto/CNCryptoShift.inc"


#include "CNCrypto/dtor_CNCryptoShift.inc"


#include "CNCrypto/CNCryptoShift_1.inc"


CNCryptoShift& CNCryptoShift::operator=(const CNCryptoShift& other)
{
    return *this;
}

#include "CNCrypto/SetMode.inc"


#include "CNCrypto/Initialize.inc"


#include "CNCrypto/Encrypt.inc"


#include "CNCrypto/Decrypt.inc"


#include "CNCrypto/Encrypt_1.inc"


#include "CNCrypto/Decrypt_1.inc"


#include "CNCrypto/GetKeySize.inc"


#include "CNCrypto/GetBlockSize.inc"


// ============================================================================
// CNCryptoRijndael（AES/Rijndael 封装：块 16 / 钥 16）
// ============================================================================

#include "CNCrypto/CNCryptoRijndael.inc"


#include "CNCrypto/dtor_CNCryptoRijndael.inc"


#include "CNCrypto/CNCryptoRijndael_1.inc"


CNCryptoRijndael& CNCryptoRijndael::operator=(const CNCryptoRijndael& other)
{
    return *this;
}

#include "CNCrypto/SetMode_1.inc"


#include "CNCrypto/Initialize_1.inc"


#include "CNCrypto/Encrypt_2.inc"


#include "CNCrypto/Decrypt_2.inc"


#include "CNCrypto/Encrypt_3.inc"


#include "CNCrypto/Decrypt_3.inc"


#include "CNCrypto/GetKeySize_1.inc"


#include "CNCrypto/GetBlockSize_1.inc"


// ============================================================================
// CNCryptoTea（TEA 封装：块 16 / 钥 16，底层 CTEA）
// ============================================================================

#include "CNCrypto/CNCryptoTea.inc"


#include "CNCrypto/dtor_CNCryptoTea.inc"


#include "CNCrypto/CNCryptoTea_1.inc"


CNCryptoTea& CNCryptoTea::operator=(const CNCryptoTea& other)
{
    return *this;
}

#include "CNCrypto/SetMode_2.inc"


#include "CNCrypto/Initialize_2.inc"


#include "CNCrypto/Encrypt_4.inc"


#include "CNCrypto/Decrypt_4.inc"


#include "CNCrypto/Encrypt_5.inc"


#include "CNCrypto/Decrypt_5.inc"


#include "CNCrypto/GetKeySize_2.inc"


#include "CNCrypto/GetBlockSize_2.inc"


// ============================================================================
// CNCryptoBlowFish（Blowfish 封装：块 8 / 钥 56）
// ============================================================================

#include "CNCrypto/CNCryptoBlowFish.inc"


#include "CNCrypto/dtor_CNCryptoBlowFish.inc"


#include "CNCrypto/CNCryptoBlowFish_1.inc"


CNCryptoBlowFish& CNCryptoBlowFish::operator=(const CNCryptoBlowFish& other)
{
    return *this;
}

#include "CNCrypto/SetMode_3.inc"


#include "CNCrypto/Initialize_3.inc"


#include "CNCrypto/Encrypt_6.inc"


#include "CNCrypto/Decrypt_6.inc"


#include "CNCrypto/Encrypt_7.inc"


#include "CNCrypto/Decrypt_7.inc"


#include "CNCrypto/GetKeySize_3.inc"


#include "CNCrypto/GetBlockSize_3.inc"


// ============================================================================
// CNCryptoRc6（RC6 封装：块 16 / 钥 60；2 参数版 ORIG 直接返回不支持）
// ============================================================================

#include "CNCrypto/CNCryptoRc6.inc"


#include "CNCrypto/dtor_CNCryptoRc6.inc"


#include "CNCrypto/CNCryptoRc6_1.inc"


CNCryptoRc6& CNCryptoRc6::operator=(const CNCryptoRc6& other)
{
    return *this;
}

#include "CNCrypto/SetMode_4.inc"


#include "CNCrypto/Initialize_4.inc"


#include "CNCrypto/Encrypt_8.inc"


#include "CNCrypto/Decrypt_8.inc"


#include "CNCrypto/Encrypt_9.inc"


#include "CNCrypto/Decrypt_9.inc"


#include "CNCrypto/GetKeySize_4.inc"


#include "CNCrypto/GetBlockSize_4.inc"


// ============================================================================
// LibTomCrypt 风格封装（Anubis/Cast5/Kasumi/Khazad/Multi2/Noekeon/
// Skipjack/TwoFish/XTea）：统一 ECB 逐块处理，算法/上下文见 CNCryptoAlgo
// ============================================================================

#define LTC_WRAPPER_DEF(Name, CtorType, BlockSize, KeySize, CtxSize, Rounds, KeyErr, FirstErr, SecondErr, LenFirst, SetupFn, EncFn, DecFn) \
Name::Name() : ICryptoGraph(CtorType)                                                       \
{                                                                                           \
    m_pCrypto = 0;                                                                          \
    symmetric_key* c = (symmetric_key*)::operator new(CtxSize);                           \
    m_pCrypto = c;                                                                          \
    if (c)                                                                                  \
    {                                                                                       \
        unsigned char key[KeySize];                                                         \
        GenerateRandomKey(key, KeySize);                                                    \
        Initialize(key, KeySize);                                                           \
    }                                                                                       \
}                                                                                           \
Name::~Name()                                                                               \
{                                                                                           \
    if (m_pCrypto)                                                                          \
    {                                                                                       \
        ::operator delete(m_pCrypto);                                                     \
        m_pCrypto = 0;                                                                      \
    }                                                                                       \
}                                                                                           \
Name::Name(const Name& other) : ICryptoGraph(CtorType)                                      \
{                                                                                           \
    m_pCrypto = 0;                                                                          \
}                                                                                           \
Name& Name::operator=(const Name& other)                                                    \
{                                                                                           \
    return *this;                                                                           \
}                                                                                           \
int Name::SetMode(CRYPTO_MODE_TYPE mode)                                                     \
{                                                                                           \
    return mode == 0 ? 0x6fffffff : 0x70000017;                                             \
}                                                                                           \
int Name::Initialize(const unsigned char* key, int keyLen)                                  \
{                                                                                           \
    if (!m_pCrypto)                                                                         \
        return 0x7000000c;                                                                  \
    if (GetKeySize() > keyLen)                                                              \
        return KeyErr;                                                                      \
    symmetric_key* c = (symmetric_key*)m_pCrypto;                                            \
    int err = SetupFn(key, GetKeySize(), Rounds, c);                                        \
    if (err == CRYPT_OK)                                                                    \
        return 0x6fffffff;                                                                  \
    if (err == FirstErr)                                                                    \
        return FirstErr == CRYPT_INVALID_KEYSIZE ? 0x70000002 : 0x7000001a;                 \
    if (err == SecondErr)                                                                   \
        return SecondErr == CRYPT_INVALID_KEYSIZE ? 0x70000002 : 0x7000001a;                \
    return 0x70000000;                                                                      \
}                                                                                           \
int Name::Encrypt(unsigned char* buf, int len)                                              \
{                                                                                           \
    if (!m_pCrypto)                                                                         \
        return 0x7000000c;                                                                  \
    if (LenFirst)                                                                           \
    {                                                                                       \
        if (len <= 0)                                                                       \
            return 0x7000000a;                                                              \
    }                                                                                       \
    int blockSize = GetBlockSize();                                                         \
    if (len % blockSize != 0)                                                               \
        return 0x70000006;                                                                  \
    if (!LenFirst)                                                                          \
    {                                                                                       \
        if (len <= 0)                                                                       \
            return 0x7000000a;                                                              \
    }                                                                                       \
    symmetric_key* c = (symmetric_key*)m_pCrypto;                                            \
    int count = len / GetBlockSize();                                                       \
    for (int i = 0; i < count; ++i)                                                         \
    {                                                                                       \
        EncFn(buf, buf, c);                                                                 \
        buf += GetBlockSize();                                                              \
    }                                                                                       \
    return 0x6fffffff;                                                                      \
}                                                                                           \
int Name::Decrypt(unsigned char* buf, int len)                                              \
{                                                                                           \
    if (!m_pCrypto)                                                                         \
        return 0x7000000c;                                                                  \
    if (LenFirst)                                                                           \
    {                                                                                       \
        if (len <= 0)                                                                       \
            return 0x7000000a;                                                              \
    }                                                                                       \
    int blockSize = GetBlockSize();                                                         \
    if (len % blockSize != 0)                                                               \
        return 0x70000006;                                                                  \
    if (!LenFirst)                                                                          \
    {                                                                                       \
        if (len <= 0)                                                                       \
            return 0x7000000a;                                                              \
    }                                                                                       \
    symmetric_key* c = (symmetric_key*)m_pCrypto;                                            \
    int count = len / GetBlockSize();                                                       \
    for (int i = 0; i < count; ++i)                                                         \
    {                                                                                       \
        DecFn(buf, buf, c);                                                                 \
        buf += GetBlockSize();                                                              \
    }                                                                                       \
    return 0x6fffffff;                                                                      \
}                                                                                           \
int Name::Encrypt(const unsigned char* in, int inLen, unsigned char* out, int outCap)       \
{                                                                                           \
    if (!m_pCrypto)                                                                         \
        return 0x7000000c;                                                                  \
    int blockSize = GetBlockSize();                                                         \
    if (inLen % blockSize != 0)                                                             \
        return 0x70000006;                                                                  \
    if (inLen > outCap)                                                                     \
        return 0x7000000a;                                                                  \
    symmetric_key* c = (symmetric_key*)m_pCrypto;                                            \
    int count = inLen / GetBlockSize();                                                     \
    for (int i = 0; i < count; ++i)                                                         \
    {                                                                                       \
        EncFn(in, out, c);                                                                  \
        in += GetBlockSize();                                                               \
        out += GetBlockSize();                                                              \
    }                                                                                       \
    return 0x6fffffff;                                                                      \
}                                                                                           \
int Name::Decrypt(const unsigned char* in, int inLen, unsigned char* out, int outCap)       \
{                                                                                           \
    if (!m_pCrypto)                                                                         \
        return 0x7000000c;                                                                  \
    int blockSize = GetBlockSize();                                                         \
    if (inLen % blockSize != 0)                                                             \
        return 0x70000006;                                                                  \
    if (inLen > outCap)                                                                     \
        return 0x7000000a;                                                                  \
    symmetric_key* c = (symmetric_key*)m_pCrypto;                                            \
    int count = inLen / GetBlockSize();                                                     \
    for (int i = 0; i < count; ++i)                                                         \
    {                                                                                       \
        DecFn(in, out, c);                                                                  \
        in += GetBlockSize();                                                               \
        out += GetBlockSize();                                                              \
    }                                                                                       \
    return 0x6fffffff;                                                                      \
}                                                                                           \
int Name::GetKeySize() const                                                                \
{                                                                                           \
    return KeySize;                                                                         \
}                                                                                           \
int Name::GetBlockSize() const                                                              \
{                                                                                           \
    return BlockSize;                                                                       \
}

LTC_WRAPPER_DEF(CNCryptoAnubis,  CRYPTO_GRAPH_ANUBIS,    0x10, 0x10, 0x268, 0x0c, 0x70000002, CRYPT_INVALID_ROUNDS,   CRYPT_INVALID_KEYSIZE, 1, anubis_setup,   anubis_ecb_encrypt,   anubis_ecb_decrypt)
LTC_WRAPPER_DEF(CNCryptoCast5,   CRYPTO_GRAPH_CAST5,     8,    0x10, 0x84,  0x10, 0x7000000a, CRYPT_INVALID_KEYSIZE,  CRYPT_INVALID_ROUNDS,  0, cast5_setup,    cast5_ecb_encrypt,    cast5_ecb_decrypt)
LTC_WRAPPER_DEF(CNCryptoKasumi,  CRYPTO_GRAPH_KASUMI,    8,    0x10, 0x100, 0x08, 0x7000000a, CRYPT_INVALID_KEYSIZE,  CRYPT_INVALID_ROUNDS,  0, kasumi_setup,   kasumi_ecb_encrypt,   kasumi_ecb_decrypt)
LTC_WRAPPER_DEF(CNCryptoKhazad,  CRYPTO_GRAPH_KHAZAD,    8,    0x10, 0x90,  0x08, 0x7000000a, CRYPT_INVALID_KEYSIZE,  CRYPT_INVALID_ROUNDS,  0, khazad_setup,   khazad_ecb_encrypt,   khazad_ecb_decrypt)
LTC_WRAPPER_DEF(CNCryptoMulti2,  CRYPTO_GRAPH_MULTI2,    8,    0x28, 0x24,  0x80, 0x7000000a, CRYPT_INVALID_KEYSIZE,  CRYPT_INVALID_ROUNDS,  0, multi2_setup,   multi2_ecb_encrypt,   multi2_ecb_decrypt)
LTC_WRAPPER_DEF(CNCryptoNoekeon, CRYPTO_GRAPH_NOEKEON,   0x10, 0x10, 0x20,  0x10, 0x7000000a, CRYPT_INVALID_KEYSIZE,  CRYPT_INVALID_ROUNDS,  0, noekeon_setup,  noekeon_ecb_encrypt,  noekeon_ecb_decrypt)
LTC_WRAPPER_DEF(CNCryptoSkipjack,CRYPTO_GRAPH_SKIPJACK,  8,    0x0a, 0x10,  0x20, 0x7000000a, CRYPT_INVALID_KEYSIZE,  CRYPT_INVALID_ROUNDS,  0, skipjack_setup, skipjack_ecb_encrypt, skipjack_ecb_decrypt)
LTC_WRAPPER_DEF(CNCryptoTwoFish, CRYPTO_GRAPH_TWOFISH,   0x10, 0x20, 0x10a0,0x10, 0x70000002, CRYPT_INVALID_ROUNDS,   CRYPT_INVALID_KEYSIZE, 0, twofish_setup,  twofish_ecb_encrypt,  twofish_ecb_decrypt)
LTC_WRAPPER_DEF(CNCryptoXTea,    CRYPTO_GRAPH_XTEA,      8,    0x10, 0x100, 0x20, 0x7000000a, CRYPT_INVALID_KEYSIZE,  CRYPT_INVALID_ROUNDS,  0, xtea_setup,     xtea_ecb_encrypt,     xtea_ecb_decrypt)

#undef LTC_WRAPPER_DEF
