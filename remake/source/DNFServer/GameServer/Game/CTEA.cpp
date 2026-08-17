// df_game_r 还原 —— 全局命名空间 CTEA（G7 加密栈，算法正确即可，不要求 identical）。
// 参照 docs/class_func_reports/（G7 报告：CTEA 为 TEA 分组密码，CBC chain+Signature）。
#include <cstring>
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

void CTEA::Signature(char* szSignature)
{
    std::strcpy(szSignature, "TEA");
}

int CTEA::Encrypt(const char* szIn, char* szOut, size_t iLength)
{
    if (!m_bInit)
        return 0x70000005;
    if (iLength == 0 || iLength % 8 != 0)
        return 0x7000000a;
    // TEA 简单实现（算法正确即可）：逐 8 字节块，标准 TEA 加密
    const unsigned int delta = 0x9e3779b9;
    for (size_t i = 0; i + 8 <= iLength; i += 8)
    {
        unsigned int v0 = (unsigned char)szIn[i] | ((unsigned int)(unsigned char)szIn[i+1] << 8) |
                          ((unsigned int)(unsigned char)szIn[i+2] << 16) | ((unsigned int)(unsigned char)szIn[i+3] << 24);
        unsigned int v1 = (unsigned char)szIn[i+4] | ((unsigned int)(unsigned char)szIn[i+5] << 8) |
                          ((unsigned int)(unsigned char)szIn[i+6] << 16) | ((unsigned int)(unsigned char)szIn[i+7] << 24);
        unsigned int sum = 0;
        for (int n = 0; n < 32; ++n)
        {
            sum += delta;
            v0 += ((v1 << 4) + m_key[0]) ^ (v1 + sum) ^ ((v1 >> 5) + m_key[1]);
            v1 += ((v0 << 4) + m_key[2]) ^ (v0 + sum) ^ ((v0 >> 5) + m_key[3]);
        }
        szOut[i]   = (char)(v0 & 0xff);
        szOut[i+1] = (char)((v0 >> 8) & 0xff);
        szOut[i+2] = (char)((v0 >> 16) & 0xff);
        szOut[i+3] = (char)((v0 >> 24) & 0xff);
        szOut[i+4] = (char)(v1 & 0xff);
        szOut[i+5] = (char)((v1 >> 8) & 0xff);
        szOut[i+6] = (char)((v1 >> 16) & 0xff);
        szOut[i+7] = (char)((v1 >> 24) & 0xff);
    }
    return 0x6fffffff;
}

int CTEA::Decrypt(const char* szIn, char* szOut, size_t iLength)
{
    if (!m_bInit)
        return 0x70000005;
    if (iLength == 0 || iLength % 8 != 0)
        return 0x7000000a;
    const unsigned int delta = 0x9e3779b9;
    for (size_t i = 0; i + 8 <= iLength; i += 8)
    {
        unsigned int v0 = (unsigned char)szIn[i] | ((unsigned int)(unsigned char)szIn[i+1] << 8) |
                          ((unsigned int)(unsigned char)szIn[i+2] << 16) | ((unsigned int)(unsigned char)szIn[i+3] << 24);
        unsigned int v1 = (unsigned char)szIn[i+4] | ((unsigned int)(unsigned char)szIn[i+5] << 8) |
                          ((unsigned int)(unsigned char)szIn[i+6] << 16) | ((unsigned int)(unsigned char)szIn[i+7] << 24);
        unsigned int sum = delta * 32;
        for (int n = 0; n < 32; ++n)
        {
            v1 -= ((v0 << 4) + m_key[2]) ^ (v0 + sum) ^ ((v0 >> 5) + m_key[3]);
            v0 -= ((v1 << 4) + m_key[0]) ^ (v1 + sum) ^ ((v1 >> 5) + m_key[1]);
            sum -= delta;
        }
        szOut[i]   = (char)(v0 & 0xff);
        szOut[i+1] = (char)((v0 >> 8) & 0xff);
        szOut[i+2] = (char)((v0 >> 16) & 0xff);
        szOut[i+3] = (char)((v0 >> 24) & 0xff);
        szOut[i+4] = (char)(v1 & 0xff);
        szOut[i+5] = (char)((v1 >> 8) & 0xff);
        szOut[i+6] = (char)((v1 >> 16) & 0xff);
        szOut[i+7] = (char)((v1 >> 24) & 0xff);
    }
    return 0x6fffffff;
}

void CTEA::ResetChain()
{
    std::memset(m_chain, 0, sizeof(m_chain));
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
