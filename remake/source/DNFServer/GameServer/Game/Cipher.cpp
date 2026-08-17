// df_game_r Cipher（G7 分组算法容器）还原（2026-08-16）。
// 依据 docs/class_func_reports/Cipher.md、ICryptoGraph.md、_lt_global_gt_.md：
//   - ctor 建 14 个 ICryptoGraph 并累加 GetKeySize 得到总钥长；
//   - Initialize 把总钥顺序分发给各图（不足则失败）；
//   - Encrypt/Decrypt 按 key % 14 选图，长度先向上对齐到块大小；
//   - 成功返回 1（0x6fffffff 由各图返回）。

#include "Cipher.h"

#include <cstdlib>
#include <cstring>

#include "CNCrypto.h"

// ============================================================================
// ICryptoGraph
// ============================================================================

ICryptoGraph::ICryptoGraph(CRYPTO_GRAPH_TYPE type)
    : m_apiType((unsigned int)type)
{
}

ICryptoGraph::ICryptoGraph(const ICryptoGraph& other)
    : m_apiType(other.m_apiType)
{
}

ICryptoGraph::~ICryptoGraph()
{
}

ICryptoGraph& ICryptoGraph::operator=(const ICryptoGraph& other)
{
    if (this != &other)
        m_apiType = other.m_apiType;
    return *this;
}

int ICryptoGraph::GetApiType() const
{
    return (int)m_apiType;
}

// ============================================================================
// Cipher
// ============================================================================

Cipher::Cipher()
    : m_totalKeyLength(0)
{
    std::memset(m_keyBuf, 0, sizeof(m_keyBuf));
    m_graphs.reserve(CRYPTO_GRAPH_MAX);
    for (int i = 0; i < CRYPTO_GRAPH_MAX; ++i)
    {
        ICryptoGraph* graph = CreateCryptoGraph((CRYPTO_GRAPH_TYPE)i);
        m_graphs.push_back(graph);
        if (graph)
            m_totalKeyLength += graph->GetKeySize();
    }
}

Cipher::~Cipher()
{
    for (size_t i = 0; i < m_graphs.size(); ++i)
    {
        DestroyCryptoGraph(m_graphs[i]);
    }
    m_graphs.clear();
}

Cipher::Cipher(const Cipher& other)
    : m_totalKeyLength(0)
{
    std::memset(m_keyBuf, 0, sizeof(m_keyBuf));
}

Cipher& Cipher::operator=(const Cipher& other)
{
    return *this;
}

int Cipher::GetTotalKeyLength() const
{
    return m_totalKeyLength;
}

int Cipher::Initialize(const unsigned char* key, int keyLen)
{
    std::memcpy(m_keyBuf, key, keyLen);
    const unsigned char* p = key;
    int remain = keyLen;
    for (int i = 0; i < CRYPTO_GRAPH_MAX; ++i)
    {
        ICryptoGraph* graph = m_graphs[i];
        if (!graph)
            return 0;
        int ks = graph->GetKeySize();
        if (remain < ks)
            return 0;
        graph->Initialize(p, ks);
        p += ks;
        remain -= ks;
    }
    return 1;
}

void Cipher::GetOriginalKey(unsigned char* dst) const
{
    std::memcpy(dst, m_keyBuf, m_totalKeyLength);
}

void Cipher::AdjustDataSize(int key, int* size) const
{
    ICryptoGraph* graph = m_graphs[GetCryptoType(key)];
    if (!graph)
        return;
    int bs = graph->GetBlockSize();
    if (*size % bs != 0)
        *size += bs - *size % bs;
}

int Cipher::Encrypt(int key, const unsigned char* src, int srcLen,
                    unsigned char* dst, int* dstLen) const
{
    if (srcLen < 1)
    {
        *dstLen = 0;
        return 1;
    }
    ICryptoGraph* graph = m_graphs[GetCryptoType(key)];
    if (!graph)
        return 0;
    int bs = graph->GetBlockSize();
    if (srcLen % bs != 0)
        srcLen += bs - srcLen % bs;
    if (graph->Encrypt(src, srcLen, dst, *dstLen) == 0x6fffffff)
    {
        *dstLen = srcLen;
        return 1;
    }
    return 0;
}

int Cipher::Decrypt(int key, const unsigned char* src, int srcLen,
                    unsigned char* dst, int* dstLen) const
{
    if (srcLen < 1)
    {
        *dstLen = 0;
        return 1;
    }
    ICryptoGraph* graph = m_graphs[GetCryptoType(key)];
    if (!graph)
        return 0;
    int bs = graph->GetBlockSize();
    (void)bs;   // ORIG Decrypt 不调整长度，直接调用
    if (graph->Decrypt(src, srcLen, dst, *dstLen) == 0x6fffffff)
    {
        *dstLen = srcLen;
        return 1;
    }
    return 0;
}

// ============================================================================
// 工厂 / 全局
// ============================================================================

int GetCryptoType(int key)
{
    return key % CRYPTO_GRAPH_MAX;
}

ICryptoGraph* CreateCryptoGraph(CRYPTO_GRAPH_TYPE type)
{
    switch (type)
    {
    case CRYPTO_GRAPH_SHIFT:     return new CNCryptoShift;
    case CRYPTO_GRAPH_RIJNDAEL:  return new CNCryptoRijndael;
    case CRYPTO_GRAPH_BLOWFISH:  return new CNCryptoBlowFish;
    case CRYPTO_GRAPH_RC6:       return new CNCryptoRc6;
    case CRYPTO_GRAPH_TWOFISH:   return new CNCryptoTwoFish;
    case CRYPTO_GRAPH_TEA:       return new CNCryptoTea;
    case CRYPTO_GRAPH_KASUMI:    return new CNCryptoKasumi;
    case CRYPTO_GRAPH_XTEA:      return new CNCryptoXTea;
    case CRYPTO_GRAPH_NOEKEON:   return new CNCryptoNoekeon;
    case CRYPTO_GRAPH_KHAZAD:    return new CNCryptoKhazad;
    case CRYPTO_GRAPH_CAST5:     return new CNCryptoCast5;
    case CRYPTO_GRAPH_SKIPJACK:  return new CNCryptoSkipjack;
    case CRYPTO_GRAPH_MULTI2:    return new CNCryptoMulti2;
    case CRYPTO_GRAPH_ANUBIS:    return new CNCryptoAnubis;
    default:                     return 0;
    }
}

void DestroyCryptoGraph(ICryptoGraph*& graph)
{
    if (graph)
    {
        delete graph;
        graph = 0;
    }
}

Cipher* G_Cipher()
{
    static Cipher g_cipher;
    return &g_cipher;
}

// ============================================================================
// 随机串 / 随机钥（ORIG @0x809e473 / @0x809e6b2）
// ============================================================================

void GenerateRandomString(char* out, int len)
{
    for (int i = 0; i < len - 1; ++i)
    {
        int r = std::rand();
        unsigned int v = (unsigned int)r >> 2;
        if (r % 3 == 0)
            out[i] = (char)('A' + (v % 0x1a));
        else if (r % 3 == 1)
            out[i] = (char)('a' + (v % 0x1a));
        else
            out[i] = (char)('0' + (v % 10));
    }
    out[len - 1] = '\0';
}

void GenerateRandomKey(unsigned char* out, int len)
{
    for (int i = 0; i < len; ++i)
        out[i] = (unsigned char)std::rand();
}
