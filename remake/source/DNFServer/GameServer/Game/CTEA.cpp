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

CTEA::CTEA()
{
    m_blockSize = 8;
    std::memset(m_key, 0, sizeof(m_key));
    std::memset(m_iv, 0, sizeof(m_iv));
    std::memset(m_chain, 0, sizeof(m_chain));
    std::memset(m_chain2, 0, sizeof(m_chain2));
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

int CTEA::Initialize(const char* key, int keyLen, const char* iv, int ivLen, int mode)
{
    if (key == 0)
        return 0x70000001;
    if (keyLen < 1)
        return 0x70000002;
    if (mode < 0 || mode > 2)
        return 0x70000003;
    m_mode = mode;
    // key 循环填充到 16 字节（ORIG：local_28[i] = key[i % keyLen]）
    unsigned char cycled[0x10];
    int src = 0;
    for (int i = 0; i < 0x10; ++i)
    {
        cycled[i] = (unsigned char)key[src];
        src = (src + 1) % keyLen;
    }
    std::memcpy(m_key, cycled, sizeof(m_key));
    if (iv && ivLen > 0)
    {
        std::memcpy(m_iv, iv, ivLen < 0x10 ? ivLen : 0x10);
        std::memcpy(m_chain, iv, ivLen < 0x10 ? ivLen : 0x10);
    }
    else
    {
        std::memset(m_iv, 0, sizeof(m_iv));
        std::memset(m_chain, 0, sizeof(m_chain));
    }
    m_bInit = true;
    return 0x6fffffff;
}

const char CTEA::sm_chain0[8] = {0, 0, 0, 0, 0, 0, 0, 0};
