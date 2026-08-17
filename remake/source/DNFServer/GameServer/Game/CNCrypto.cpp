// df_game_r CNCrypto* 统一分组密码封装（G7，2026-08-16 还原）。
// 接口/错误码与 docs/class_func_reports/CNCrypto*.md 一致；底层算法见
// CNCryptoAlgo.cpp（标准实现，算法正确即可）。

#include "CNCrypto.h"

#include <cstring>

#include "CRijndael.h"
#include "CEnvironment.h"   // CTEA
#include "CNCryptoAlgo.h"

// ============================================================================
// CNCryptoShift（SHIFT：32 位 ROR + XOR，块 4 / 钥 8）
// ============================================================================

CNCryptoShift::CNCryptoShift()
    : ICryptoGraph(CRYPTO_GRAPH_SHIFT)
{
    m_shift = 7;
    m_key = 0x1d3b4e9cU;
}

CNCryptoShift::~CNCryptoShift()
{
}

CNCryptoShift::CNCryptoShift(const CNCryptoShift& other)
    : ICryptoGraph(CRYPTO_GRAPH_SHIFT)
{
    m_shift = other.m_shift;
    m_key = other.m_key;
}

CNCryptoShift& CNCryptoShift::operator=(const CNCryptoShift& other)
{
    if (this != &other)
    {
        m_shift = other.m_shift;
        m_key = other.m_key;
    }
    return *this;
}

int CNCryptoShift::SetMode(int)
{
    return 0x70000017;
}

int CNCryptoShift::Initialize(const unsigned char* key, int keyLen)
{
    if (keyLen < 8)
        return 0;
    m_shift = (unsigned int)key[0] | ((unsigned int)key[1] << 8) |
              ((unsigned int)key[2] << 16) | ((unsigned int)key[3] << 24);
    m_shift &= 0x1f;
    m_key = (unsigned int)key[4] | ((unsigned int)key[5] << 8) |
            ((unsigned int)key[6] << 16) | ((unsigned int)key[7] << 24);
    return 0x6fffffff;
}

int CNCryptoShift::Encrypt(unsigned char* buf, int len)
{
    if (m_shift >= 0x20)
        return 0x7000000c;
    if (len % 4 != 0)
        return 0x70000006;
    unsigned int* p = (unsigned int*)buf;
    for (int i = 0; i < len / 4; ++i)
    {
        unsigned int v = p[i];
        v = RORc(v, (int)m_shift);
        p[i] = v ^ m_key;
    }
    return 0x6fffffff;
}

int CNCryptoShift::Decrypt(unsigned char* buf, int len)
{
    if (m_shift >= 0x20)
        return 0x7000000c;
    if (len % 4 != 0)
        return 0x70000006;
    unsigned int* p = (unsigned int*)buf;
    for (int i = 0; i < len / 4; ++i)
    {
        unsigned int v = p[i] ^ m_key;
        p[i] = ROLc(v, (int)m_shift);
    }
    return 0x6fffffff;
}

int CNCryptoShift::Encrypt(const unsigned char* in, int inLen,
                           unsigned char* out, int outCap)
{
    if (outCap < inLen)
        return 0x7000000b;
    std::memcpy(out, in, inLen);
    return Encrypt(out, inLen);
}

int CNCryptoShift::Decrypt(const unsigned char* in, int inLen,
                           unsigned char* out, int outCap)
{
    if (outCap < inLen)
        return 0x7000000b;
    std::memcpy(out, in, inLen);
    return Decrypt(out, inLen);
}

int CNCryptoShift::GetKeySize() const
{
    return 8;
}

int CNCryptoShift::GetBlockSize() const
{
    return 4;
}

// ============================================================================
// CNCryptoRijndael（AES/Rijndael 封装：块 16 / 钥 16）
// ============================================================================

CNCryptoRijndael::CNCryptoRijndael()
    : ICryptoGraph(CRYPTO_GRAPH_RIJNDAEL)
{
    m_pCrypto = 0;
    CRijndael* c = new CRijndael;
    m_pCrypto = c;
    if (c)
    {
        char key[0x80];
        GenerateRandomString(key, 0x80);
        if (c->Initialize(key, 0x80, CRijndael::sm_chain0, 0x10, 0x10, 0, 0) != 0x6fffffff)
        {
            delete c;
            m_pCrypto = 0;
        }
    }
}

CNCryptoRijndael::~CNCryptoRijndael()
{
    if (m_pCrypto)
    {
        delete (CRijndael*)m_pCrypto;
        m_pCrypto = 0;
    }
}

CNCryptoRijndael::CNCryptoRijndael(const CNCryptoRijndael& other)
    : ICryptoGraph(CRYPTO_GRAPH_RIJNDAEL)
{
    m_pCrypto = 0;
}

CNCryptoRijndael& CNCryptoRijndael::operator=(const CNCryptoRijndael& other)
{
    return *this;
}

int CNCryptoRijndael::SetMode(int mode)
{
    if (!m_pCrypto)
        return 0x7000000c;
    int m = -1;
    if (mode == 1)
        m = 1;
    else if (mode == 3)
        m = 2;
    else if (mode == 0)
        m = 0;
    else
        return 0x70000017;
    ((IMethod*)m_pCrypto)->SetMode(m);
    return 0x6fffffff;
}

int CNCryptoRijndael::Initialize(const unsigned char* key, int keyLen)
{
    if (!m_pCrypto)
        return 0x7000000c;
    return ((CRijndael*)m_pCrypto)->Initialize(
        (const char*)key, keyLen, CRijndael::sm_chain0, 0x10, 0x10, 0, 0);
}

int CNCryptoRijndael::Encrypt(unsigned char* buf, int len)
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (len % 0x10 != 0)
        return 0x70000006;
    return ((CRijndael*)m_pCrypto)->Encrypt((const char*)buf, (char*)buf, len);
}

int CNCryptoRijndael::Decrypt(unsigned char* buf, int len)
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (len % 0x10 != 0)
        return 0x70000006;
    return ((CRijndael*)m_pCrypto)->Decrypt((const char*)buf, (char*)buf, len);
}

int CNCryptoRijndael::Encrypt(const unsigned char* in, int inLen,
                              unsigned char* out, int outCap)
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (outCap < inLen)
        return 0x7000000b;
    if (inLen % 0x10 != 0)
        return 0x70000006;
    return ((CRijndael*)m_pCrypto)->Encrypt((const char*)in, (char*)out, inLen);
}

int CNCryptoRijndael::Decrypt(const unsigned char* in, int inLen,
                              unsigned char* out, int outCap)
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (outCap < inLen)
        return 0x7000000b;
    if (inLen % 0x10 != 0)
        return 0x70000006;
    return ((CRijndael*)m_pCrypto)->Decrypt((const char*)in, (char*)out, inLen);
}

int CNCryptoRijndael::GetKeySize() const
{
    return 0x10;
}

int CNCryptoRijndael::GetBlockSize() const
{
    return 0x10;
}

// ============================================================================
// CNCryptoTea（TEA 封装：块 16 / 钥 16，底层 CTEA）
// ============================================================================

CNCryptoTea::CNCryptoTea()
    : ICryptoGraph(CRYPTO_GRAPH_TEA)
{
    m_pCrypto = 0;
    CTEA* c = new CTEA;
    m_pCrypto = c;
    if (c)
    {
        char key[0x80];
        GenerateRandomString(key, 0x80);
        if (c->Initialize(key, 0x80, "", 0, 0) != 0x6fffffff)
        {
            delete c;
            m_pCrypto = 0;
        }
    }
}

CNCryptoTea::~CNCryptoTea()
{
    if (m_pCrypto)
    {
        delete (CTEA*)m_pCrypto;
        m_pCrypto = 0;
    }
}

CNCryptoTea::CNCryptoTea(const CNCryptoTea& other)
    : ICryptoGraph(CRYPTO_GRAPH_TEA)
{
    m_pCrypto = 0;
}

CNCryptoTea& CNCryptoTea::operator=(const CNCryptoTea& other)
{
    return *this;
}

int CNCryptoTea::SetMode(int mode)
{
    if (!m_pCrypto)
        return 0x7000000c;
    int m = -1;
    if (mode == 1)
        m = 1;
    else if (mode == 3)
        m = 2;
    else if (mode == 0)
        m = 0;
    else
        return 0x70000017;
    ((CTEA*)m_pCrypto)->m_mode = m;
    return 0x6fffffff;
}

int CNCryptoTea::Initialize(const unsigned char* key, int keyLen)
{
    if (!m_pCrypto)
        return 0x7000000c;
    return ((CTEA*)m_pCrypto)->Initialize((const char*)key, keyLen, "", 0, 0);
}

int CNCryptoTea::Encrypt(unsigned char* buf, int len)
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (len < 1)
        return 0x7000000b;
    if (len % 0x10 != 0)
        return 0x70000006;
    return ((CTEA*)m_pCrypto)->Encrypt((const char*)buf, (char*)buf, len);
}

int CNCryptoTea::Decrypt(unsigned char* buf, int len)
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (len < 1)
        return 0x7000000b;
    if (len % 0x10 != 0)
        return 0x70000006;
    return ((CTEA*)m_pCrypto)->Decrypt((const char*)buf, (char*)buf, len);
}

int CNCryptoTea::Encrypt(const unsigned char* in, int inLen,
                         unsigned char* out, int outCap)
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (outCap < inLen)
        return 0x7000000b;
    if (inLen % 0x10 != 0)
        return 0x70000006;
    return ((CTEA*)m_pCrypto)->Encrypt((const char*)in, (char*)out, inLen);
}

int CNCryptoTea::Decrypt(const unsigned char* in, int inLen,
                         unsigned char* out, int outCap)
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (outCap < inLen)
        return 0x7000000b;
    if (inLen % 0x10 != 0)
        return 0x70000006;
    return ((CTEA*)m_pCrypto)->Decrypt((const char*)in, (char*)out, inLen);
}

int CNCryptoTea::GetKeySize() const
{
    return 0x10;
}

int CNCryptoTea::GetBlockSize() const
{
    return 0x10;
}

// ============================================================================
// CNCryptoBlowFish（Blowfish 封装：块 8 / 钥 56）
// ============================================================================

CNCryptoBlowFish::CNCryptoBlowFish()
    : ICryptoGraph(CRYPTO_GRAPH_BLOWFISH)
{
    m_pCrypto = 0;
    m_mode = 0;
    symmetric_key* c = new symmetric_key;
    m_pCrypto = c;
    if (c)
    {
        unsigned char key[0x38];
        GenerateRandomKey(key, 0x38);
        if (blowfish_setup(key, 0x38, 0, c) != CRYPT_OK)
        {
            delete c;
            m_pCrypto = 0;
        }
    }
}

CNCryptoBlowFish::~CNCryptoBlowFish()
{
    if (m_pCrypto)
    {
        delete (symmetric_key*)m_pCrypto;
        m_pCrypto = 0;
    }
}

CNCryptoBlowFish::CNCryptoBlowFish(const CNCryptoBlowFish& other)
    : ICryptoGraph(CRYPTO_GRAPH_BLOWFISH)
{
    m_pCrypto = 0;
    m_mode = other.m_mode;
}

CNCryptoBlowFish& CNCryptoBlowFish::operator=(const CNCryptoBlowFish& other)
{
    if (this != &other)
        m_mode = other.m_mode;
    return *this;
}

int CNCryptoBlowFish::SetMode(int mode)
{
    m_mode = (unsigned int)mode;
    return 0x6fffffff;
}

int CNCryptoBlowFish::Initialize(const unsigned char* key, int keyLen)
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (keyLen < 0x38)
        return 0x7000000a;
    int err = blowfish_setup(key, 0x38, 0, (symmetric_key*)m_pCrypto);
    if (err == CRYPT_OK)
        return 0x6fffffff;
    if (err == CRYPT_INVALID_ROUNDS)
        return 0x7000001a;
    if (err == CRYPT_INVALID_KEYSIZE)
        return 0x70000002;
    return 0x70000000;
}

int CNCryptoBlowFish::Encrypt(unsigned char* buf, int len)
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (len < 1)
        return 0x7000000a;
    if (len % 8 != 0)
        return 0x70000006;
    for (int i = 0; i < len / 8; ++i)
        blowfish_ecb_encrypt(buf + i * 8, buf + i * 8, (symmetric_key*)m_pCrypto);
    return 0x6fffffff;
}

int CNCryptoBlowFish::Decrypt(unsigned char* buf, int len)
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (len < 1)
        return 0x7000000a;
    if (len % 8 != 0)
        return 0x70000006;
    for (int i = 0; i < len / 8; ++i)
        blowfish_ecb_decrypt(buf + i * 8, buf + i * 8, (symmetric_key*)m_pCrypto);
    return 0x6fffffff;
}

int CNCryptoBlowFish::Encrypt(const unsigned char* in, int inLen,
                              unsigned char* out, int outCap)
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (outCap < inLen)
        return 0x7000000b;
    if (inLen % 8 != 0)
        return 0x70000006;
    for (int i = 0; i < inLen / 8; ++i)
        blowfish_ecb_encrypt(in + i * 8, out + i * 8, (symmetric_key*)m_pCrypto);
    return 0x6fffffff;
}

int CNCryptoBlowFish::Decrypt(const unsigned char* in, int inLen,
                              unsigned char* out, int outCap)
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (outCap < inLen)
        return 0x7000000b;
    if (inLen % 8 != 0)
        return 0x70000006;
    for (int i = 0; i < inLen / 8; ++i)
        blowfish_ecb_decrypt(in + i * 8, out + i * 8, (symmetric_key*)m_pCrypto);
    return 0x6fffffff;
}

int CNCryptoBlowFish::GetKeySize() const
{
    return 0x38;
}

int CNCryptoBlowFish::GetBlockSize() const
{
    return 8;
}

// ============================================================================
// CNCryptoRc6（RC6 封装：块 16 / 钥 60；2 参数版 ORIG 直接返回不支持）
// ============================================================================

CNCryptoRc6::CNCryptoRc6()
    : ICryptoGraph(CRYPTO_GRAPH_RC6)
{
    m_pCrypto = 0;
    RC6_ALG_INFO* c = new RC6_ALG_INFO;
    m_pCrypto = c;
    if (c)
    {
        unsigned char key[0x3c];
        GenerateRandomKey(key, 0x3c);
        Initialize(key, 0x3c);
    }
}

CNCryptoRc6::~CNCryptoRc6()
{
    if (m_pCrypto)
    {
        delete (RC6_ALG_INFO*)m_pCrypto;
        m_pCrypto = 0;
    }
}

CNCryptoRc6::CNCryptoRc6(const CNCryptoRc6& other)
    : ICryptoGraph(CRYPTO_GRAPH_RC6)
{
    m_pCrypto = 0;
}

CNCryptoRc6& CNCryptoRc6::operator=(const CNCryptoRc6& other)
{
    return *this;
}

int CNCryptoRc6::SetMode(int)
{
    return 0x70000017;
}

int CNCryptoRc6::Initialize(const unsigned char* key, int keyLen)
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (keyLen < 0x3c)
        return 0x70000002;
    RC6_ALG_INFO* c = (RC6_ALG_INFO*)m_pCrypto;
    std::memcpy(c->key, key, 0x20);
    rc6_key_setup(c, 0x20);
    return 0x6fffffff;
}

int CNCryptoRc6::Encrypt(unsigned char*, int)
{
    return 0x70000017;
}

int CNCryptoRc6::Decrypt(unsigned char*, int)
{
    return 0x70000017;
}

int CNCryptoRc6::Encrypt(const unsigned char* in, int inLen,
                         unsigned char* out, int outCap)
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (inLen % 0x10 != 0)
        return 0x70000006;
    if (outCap < inLen)
        return 0x7000000a;
    const unsigned int* src = (const unsigned int*)in;
    unsigned int* dst = (unsigned int*)out;
    for (int i = 0; i < inLen / 0x10; ++i)
    {
        rc6_block_encrypt((const RC6_ALG_INFO*)m_pCrypto, src, dst);
        src += 4;
        dst += 4;
    }
    return 0x6fffffff;
}

int CNCryptoRc6::Decrypt(const unsigned char* in, int inLen,
                         unsigned char* out, int outCap)
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (inLen % 0x10 != 0)
        return 0x70000006;
    if (outCap < inLen)
        return 0x7000000a;
    const unsigned int* src = (const unsigned int*)in;
    unsigned int* dst = (unsigned int*)out;
    for (int i = 0; i < inLen / 0x10; ++i)
    {
        rc6_block_decrypt((const RC6_ALG_INFO*)m_pCrypto, src, dst);
        src += 4;
        dst += 4;
    }
    return 0x6fffffff;
}

int CNCryptoRc6::GetKeySize() const
{
    return 0x3c;
}

int CNCryptoRc6::GetBlockSize() const
{
    return 0x10;
}

// ============================================================================
// LibTomCrypt 风格封装（Anubis/Cast5/Kasumi/Khazad/Multi2/Noekeon/
// Skipjack/TwoFish/XTea）：统一 ECB 逐块处理，算法/上下文见 CNCryptoAlgo
// ============================================================================

#define LTC_WRAPPER_DEF(Name, CtorType, BlockSize, KeySize, CtxSize, SetupFn, EncFn, DecFn) \
Name::Name() : ICryptoGraph(CtorType)                                                       \
{                                                                                           \
    m_pCrypto = 0;                                                                          \
    symmetric_key* c = (symmetric_key*)new char[CtxSize];                                   \
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
        delete[] (char*)m_pCrypto;                                                          \
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
int Name::SetMode(int mode)                                                                 \
{                                                                                           \
    return mode == 0 ? 0x6fffffff : 0x70000017;                                             \
}                                                                                           \
int Name::Initialize(const unsigned char* key, int keyLen)                                  \
{                                                                                           \
    if (!m_pCrypto)                                                                         \
        return 0x7000000c;                                                                  \
    if (keyLen < KeySize)                                                                   \
        return 0x7000000a;                                                                  \
    int err = SetupFn(key, KeySize, 0, (symmetric_key*)m_pCrypto);                          \
    if (err == CRYPT_OK)                                                                    \
        return 0x6fffffff;                                                                  \
    if (err == CRYPT_INVALID_ROUNDS)                                                        \
        return 0x7000001a;                                                                  \
    if (err == CRYPT_INVALID_KEYSIZE)                                                       \
        return 0x70000002;                                                                  \
    return 0x70000000;                                                                      \
}                                                                                           \
int Name::Encrypt(unsigned char* buf, int len)                                              \
{                                                                                           \
    if (!m_pCrypto)                                                                         \
        return 0x7000000c;                                                                  \
    if (len < 1)                                                                            \
        return 0x7000000a;                                                                  \
    if (len % BlockSize != 0)                                                               \
        return 0x70000006;                                                                  \
    for (int i = 0; i < len / BlockSize; ++i)                                               \
        EncFn(buf + i * BlockSize, buf + i * BlockSize, (symmetric_key*)m_pCrypto);         \
    return 0x6fffffff;                                                                      \
}                                                                                           \
int Name::Decrypt(unsigned char* buf, int len)                                              \
{                                                                                           \
    if (!m_pCrypto)                                                                         \
        return 0x7000000c;                                                                  \
    if (len < 1)                                                                            \
        return 0x7000000a;                                                                  \
    if (len % BlockSize != 0)                                                               \
        return 0x70000006;                                                                  \
    for (int i = 0; i < len / BlockSize; ++i)                                               \
        DecFn(buf + i * BlockSize, buf + i * BlockSize, (symmetric_key*)m_pCrypto);         \
    return 0x6fffffff;                                                                      \
}                                                                                           \
int Name::Encrypt(const unsigned char* in, int inLen, unsigned char* out, int outCap)       \
{                                                                                           \
    if (!m_pCrypto)                                                                         \
        return 0x7000000c;                                                                  \
    if (outCap < inLen)                                                                     \
        return 0x7000000b;                                                                  \
    if (inLen % BlockSize != 0)                                                             \
        return 0x70000006;                                                                  \
    for (int i = 0; i < inLen / BlockSize; ++i)                                             \
        EncFn(in + i * BlockSize, out + i * BlockSize, (symmetric_key*)m_pCrypto);          \
    return 0x6fffffff;                                                                      \
}                                                                                           \
int Name::Decrypt(const unsigned char* in, int inLen, unsigned char* out, int outCap)       \
{                                                                                           \
    if (!m_pCrypto)                                                                         \
        return 0x7000000c;                                                                  \
    if (outCap < inLen)                                                                     \
        return 0x7000000b;                                                                  \
    if (inLen % BlockSize != 0)                                                             \
        return 0x70000006;                                                                  \
    for (int i = 0; i < inLen / BlockSize; ++i)                                             \
        DecFn(in + i * BlockSize, out + i * BlockSize, (symmetric_key*)m_pCrypto);          \
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

LTC_WRAPPER_DEF(CNCryptoAnubis,  CRYPTO_GRAPH_ANUBIS,    0x10, 0x10, 0x268, anubis_setup,   anubis_ecb_encrypt,   anubis_ecb_decrypt)
LTC_WRAPPER_DEF(CNCryptoCast5,   CRYPTO_GRAPH_CAST5,     8,    0x10, 0x84,  cast5_setup,    cast5_ecb_encrypt,    cast5_ecb_decrypt)
LTC_WRAPPER_DEF(CNCryptoKasumi,  CRYPTO_GRAPH_KASUMI,    8,    0x10, 0x100, kasumi_setup,   kasumi_ecb_encrypt,   kasumi_ecb_decrypt)
LTC_WRAPPER_DEF(CNCryptoKhazad,  CRYPTO_GRAPH_KHAZAD,    8,    0x10, 0x90,  khazad_setup,   khazad_ecb_encrypt,   khazad_ecb_decrypt)
LTC_WRAPPER_DEF(CNCryptoMulti2,  CRYPTO_GRAPH_MULTI2,    8,    0x28, 0x24,  multi2_setup,   multi2_ecb_encrypt,   multi2_ecb_decrypt)
LTC_WRAPPER_DEF(CNCryptoNoekeon, CRYPTO_GRAPH_NOEKEON,   0x10, 0x10, 0x20,  noekeon_setup,  noekeon_ecb_encrypt,  noekeon_ecb_decrypt)
LTC_WRAPPER_DEF(CNCryptoSkipjack,CRYPTO_GRAPH_SKIPJACK,  8,    0x0a, 0x10,  skipjack_setup, skipjack_ecb_encrypt, skipjack_ecb_decrypt)
LTC_WRAPPER_DEF(CNCryptoTwoFish, CRYPTO_GRAPH_TWOFISH,   0x10, 0x20, 0x10a0,twofish_setup,  twofish_ecb_encrypt,  twofish_ecb_decrypt)
LTC_WRAPPER_DEF(CNCryptoXTea,    CRYPTO_GRAPH_XTEA,      8,    0x10, 0x100, xtea_setup,     xtea_ecb_encrypt,     xtea_ecb_decrypt)

#undef LTC_WRAPPER_DEF
