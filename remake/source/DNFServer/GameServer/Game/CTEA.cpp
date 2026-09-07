// df_game_r 还原 —— 全局命名空间 CTEA（G7 加密栈，算法正确即可，不要求 identical）。
// 参照 docs/class_func_reports/（G7 报告：CTEA 为 TEA 分组密码，CBC chain+Signature）。
#include <cstring>
#include "ChecksumAlgo.h"   // CSHA（ORIG 全局类，AddData/FinalDigest）
#include "CEnvironment.h"

// ---- IMethod 抽象基类（G7 加密模式基类，算法正确即可）----
IMethod::IMethod()
    : m_bInit(false), m_blockSize(8), m_keyLength(0), m_mode(0), m_padding(0)
{
}

IMethod::~IMethod()
{
}

// ---- 全局 IMethod 工具成员（ORIG T 0x080b459e/0x080b4616；声明 CEnvironment.h:42-43 static，
// ServerLib 侧为 nsl::IMethod 不同 mangled ⇒ game 全局形定义须落本 TU——Main 按 DBMW/Method.cpp
// :134-149 语义复制，G7 算法豁免面，big-endian 字组双向转换）----
void IMethod::BytesToWord(unsigned char const* src, unsigned int& dst)
{
    dst = 0;
    dst |= (unsigned int)src[0] << 24;
    dst |= (unsigned int)src[1] << 16;
    dst |= (unsigned int)src[2] << 8;
    dst |= (unsigned int)src[3];
}
void IMethod::WordToBytes(unsigned int src, unsigned char* dst)
{
    *dst = (unsigned char)(src >> 24);
    ++dst;
    *dst = (unsigned char)(src >> 16);
    ++dst;
    *dst = (unsigned char)(src >> 8);
    ++dst;
    *dst = (unsigned char)src;
}

CTEA::CTEA()
{
    m_blockSize = 8;
    m_keyLength = 0x10;
    std::memset(m_key, 0, sizeof(m_key));
    std::memset(m_chain, 0, sizeof(m_chain));
    std::memset(m_chain2, 0, sizeof(m_chain2));
    std::memset(m_iv, 0, sizeof(m_iv));
}

CTEA::~CTEA()
{
}

// ORIG _ZN4CTEA9SignatureEPc T @0x080c35ec：
//   char buf[23] 内建清零；strcpy "TEA"；接 m_key(m_keyLength)；sprintf "%d%d"
//   拼 m_mode/m_padding；CSHA 摘要写出。
void CTEA::Signature(char* szSignature)
{
    char buf[0x17];
    std::memset(buf, 0, sizeof(buf));
    std::strcpy(buf, "TEA");
    const int headLen = static_cast<int>(std::strlen(buf));
    std::memcpy(buf + headLen, m_key, m_keyLength);
    std::sprintf(buf + headLen + m_keyLength, "%d%d", m_mode, m_padding);

    CSHA sha;
    sha.AddData(buf, static_cast<int>(std::strlen(buf)));
    sha.FinalDigest(szSignature);
}

// ---- ORIG _ZN4CTEA12EncryptBlockEPKhPh（0x080c3724）TEA 分组加密核心 ----
int CTEA::EncryptBlock(unsigned char const* src, unsigned char* dst)
{
    unsigned int v0;
    unsigned int v1;
    BytesToWord(src, v0);
    BytesToWord(src + 4, v1);
    const unsigned int* key = reinterpret_cast<const unsigned int*>(m_key);
    unsigned int sum = 0;
    const unsigned int delta = 0x9e3779b9;
    for (int n = 0; n < 32; ++n)
    {
        sum += delta;
        v0 += ((v1 << 4) + key[0]) ^ (v1 + sum) ^ ((v1 >> 5) + key[1]);
        v1 += ((v0 << 4) + key[2]) ^ (v0 + sum) ^ ((v0 >> 5) + key[3]);
    }
    WordToBytes(v0, dst);
    WordToBytes(v1, dst + 4);
    return 0;
}

// ---- ORIG _ZN4CTEA12DecryptBlockEPKhPh（0x080c3804）TEA 分组解密核心 ----
int CTEA::DecryptBlock(unsigned char const* src, unsigned char* dst)
{
    unsigned int v0;
    unsigned int v1;
    BytesToWord(src, v0);
    BytesToWord(src + 4, v1);
    const unsigned int* key = reinterpret_cast<const unsigned int*>(m_key);
    unsigned int sum = 0xc6ef3720;
    const unsigned int delta = 0x9e3779b9;
    for (int n = 0; n < 32; ++n)
    {
        v1 -= ((v0 << 4) + key[2]) ^ (v0 + sum) ^ ((v0 >> 5) + key[3]);
        v0 -= ((v1 << 4) + key[0]) ^ (v1 + sum) ^ ((v1 >> 5) + key[1]);
        sum -= delta;
    }
    WordToBytes(v0, dst);
    WordToBytes(v1, dst + 4);
    return 0;
}

// ---- ORIG _ZN4CTEA7EncryptEPKcPcj（0x080c38e4）：CBC 链式，三模式 ----
int CTEA::Encrypt(const char* szIn, char* szOut, size_t iLength)
{
    if (!m_bInit)
        return 0x70000005;
    if (iLength == 0 || iLength % m_blockSize != 0)
        return 0x7000000a;
    const char* src = szIn;
    char* out = szOut;
    size_t blocks = iLength / m_blockSize;
    for (size_t i = 0; i < blocks; ++i)
    {
        if (m_mode == 1)
        {
            IMethod::Xor((char*)m_chain2, src);                // m_chain2(+0x40)
            EncryptBlock(m_chain2, (unsigned char*)out);
            memcpy(m_chain2, out, m_blockSize);
        }
        else if (m_mode == 2)
        {
            EncryptBlock(m_chain2, (unsigned char*)out);
            IMethod::Xor(out, src);
            memcpy(m_chain2, out, m_blockSize);
        }
        else
        {
            EncryptBlock((unsigned char const*)src, (unsigned char*)out);
        }
        src += m_blockSize;
        out += m_blockSize;
    }
    return 0x6fffffff;
}

// ---- ORIG _ZN4CTEA7DecryptEPKcPcj（0x080c3afe）：CBC 链式，三模式 ----
int CTEA::Decrypt(const char* szIn, char* szOut, size_t iLength)
{
    if (!m_bInit)
        return 0x70000005;
    if (iLength == 0 || iLength % m_blockSize != 0)
        return 0x7000000a;
    const char* src = szIn;
    char* out = szOut;
    size_t blocks = iLength / m_blockSize;
    for (size_t i = 0; i < blocks; ++i)
    {
        if (m_mode == 1)
        {
            DecryptBlock((unsigned char const*)src, (unsigned char*)out);
            IMethod::Xor(out, (const char*)m_chain2);
            memcpy(m_chain2, src, m_blockSize);
        }
        else if (m_mode == 2)
        {
            EncryptBlock(m_chain2, (unsigned char*)out);
            IMethod::Xor(out, src);
            memcpy(m_chain2, src, m_blockSize);
        }
        else
        {
            DecryptBlock((unsigned char const*)src, (unsigned char*)out);
        }
        src += m_blockSize;
        out += m_blockSize;
    }
    return 0x6fffffff;
}

// ORIG _ZN4CTEA10ResetChainEv T @0x080c35a8：
//   !m_bInit → 0x70000005；否则 chain(+0x38) 复制回 chain2(+0x40)，
//   长度取 m_blockSize(this+0x08)，成功返 0x6fffffff。
int CTEA::ResetChain()
{
    if (!m_bInit)
    {
        return 0x70000005;
    }
    std::memcpy(m_chain2, m_chain, m_blockSize);
    return 0x6fffffff;
}

// [b23S3C] ORIG T 0x080c339c 全量重转写：旧形为猜测版（缺 +0x14 写/双 memcmp 复用
// 判定/BytesToWord×4），参数语义照 ORIG：第 3 参是与 m_chain2 比较的另一 key，
// 第 4/5 参分别落 +0x10/+0x14 并做 0..2 值域守卫。
int CTEA::Initialize(const char* key, int keyLen, const char* otherKey,
                     int mode1, int mode2)
{
    if (key == 0)
        return 0x70000001;
    if (keyLen < 1)
        return 0x70000002;
    if (mode1 < 0 || mode1 > 2)
        return 0x70000003;
    if (mode2 < 0 || mode2 > 2)
        return 0x70000004;

    m_mode = mode1;
    m_padding = mode2;

    // 循环填充 buf（ORIG -0x24；圈计数用 this->m_keyLength，取模用 keyLen）。
    unsigned char buf[0x10];
    int i = 0;
    int src = 0;
    for (; i < m_keyLength; src = (src + 1) % keyLen)
    {
        buf[i] = (unsigned char)key[src];
        ++i;   // [b23S3C] ORIG 0x84 i++ 在体内、src 取模在增补段（装载序实证）
    }

    bool sameKey = false;
    bool sameChain = false;
    if (m_bInit)
    {
        if (std::memcmp(m_chain, otherKey, m_blockSize) == 0)
            sameChain = true;
        if (std::memcmp(m_key, buf, m_keyLength) == 0)
            sameKey = true;
    }
    if (sameChain)
    {
        std::memcpy(m_chain2, m_chain, m_blockSize);
    }
    else
    {
        std::memcpy(m_chain, otherKey, m_blockSize);
        std::memcpy(m_chain2, otherKey, m_blockSize);
    }
    if (sameKey)
        return 0x6fffffff;

    std::memcpy(m_key, buf, m_keyLength);
    IMethod::BytesToWord(buf + 0x0, m_iv[0]);
    IMethod::BytesToWord(buf + 0x4, m_iv[1]);
    IMethod::BytesToWord(buf + 0x8, m_iv[2]);
    IMethod::BytesToWord(buf + 0xc, m_iv[3]);
    m_bInit = true;
    return 0x6fffffff;
}


const char CTEA::sm_chain0[8] = {0, 0, 0, 0, 0, 0, 0, 0};
