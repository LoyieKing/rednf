// df_game_r Cipher（G7 分组算法容器）还原（2026-08-16）。
// 依据 docs/class_func_reports/Cipher.md、ICryptoGraph.md、_lt_global_gt_.md：
//   - ctor 建 14 个 ICryptoGraph 并累加 GetKeySize 得到总钥长；
//   - Initialize 把总钥顺序分发给各图（不足则失败）；
//   - Encrypt/Decrypt 按 key % 14 选图，长度先向上对齐到块大小；
//   - 成功返回 1（0x6fffffff 由各图返回）。

#include "Cipher.h"
#include "CEnvironment.h"  // GlobalInstance<T>（G_Cipher→inst_ptr 单例模板）

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
    // [N7 封顶] push_back 元素 GetKeySize() 虚调用 callee 寄存器 %edx(ORIG)↔%eax(4.4)。
    m_graphs.reserve(CRYPTO_GRAPH_MAX);
    for (int i = 0; i < CRYPTO_GRAPH_MAX; ++i)
    {
        ICryptoGraph* graph = CreateCryptoGraph((CRYPTO_GRAPH_TYPE)i);
        m_graphs.push_back(graph);
        m_totalKeyLength += graph->GetKeySize();
    }
}

Cipher::~Cipher()
{
    for (int i = 0; i <= 0xd; ++i)
    {
        DestroyCryptoGraph(m_graphs[i]);
        m_graphs[i] = 0;
    }
    m_graphs.clear();
}

Cipher::Cipher(const Cipher& other)
{
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
    for (int i = 0; i < CRYPTO_GRAPH_MAX; ++i)
    {
        int ks = m_graphs[i]->GetKeySize();
        // [b28c-L9] ORIG 0x08097357 `cmp -0xc(%ebp),%eax; jl return0岛`：body 为
        // fall-through、`return 0` 外置岛（4.4 对否定早返恒反转成 jge + fall 岛）。
        // 正向包裹（P222）复现 ORIG 极性与岛位：ndiff 21→6。
        // 残差 6 = N7：m_graphs[i]->Initialize 间接 call 的 funcptr ORIG→%ecx、
        // 实参链→%edx，4.4 恒互换（%edx/%ecx）；指令数 60=60、助记符全等 ⇒ NEAR 封顶。
        if (keyLen >= ks)
        {
            m_graphs[i]->Initialize(key, ks);
            key += ks;
            keyLen -= ks;
        }
        else
        {
            return 0;
        }
    }
    return 1;
}

void Cipher::GetOriginalKey(unsigned char* dst)
{
    std::memcpy(dst, m_keyBuf, m_totalKeyLength);
}

void Cipher::AdjustDataSize(int key, int* size) const
{
    int type = GetCryptoType(key);
    int v = *size;
    int bs = m_graphs[type]->GetBlockSize();
    if (v % bs != 0)
        v += bs - v % bs;
    *size = v;
}

int Cipher::Encrypt(int key, const unsigned char* src, int srcLen,
                    unsigned char* dst, int* dstLen)
{
    // [N7 封顶] m_graphs[type]->Encrypt 虚调用：ORIG funcptr→%ecx/实参链→%edx，
    // 4.4 互换（funcptr→%edx/实参→%ecx），寄存器分配跨版本差。
    if (srcLen < 1)
    {
        *dstLen = 0;
        return 1;
    }
    int type = GetCryptoType(key);
    int outCap = *dstLen;   // ORIG 在选图前死读 *dstLen（-0x10 槽，后续未用）
    (void)outCap;
    int bs = m_graphs[type]->GetBlockSize();
    if (srcLen % bs != 0)
        srcLen += bs - srcLen % bs;
    if (m_graphs[type]->Encrypt(src, srcLen, dst, *dstLen) == 0x6fffffff)
    {
        *dstLen = srcLen;
        return 1;
    }
    return 0;
}

int Cipher::Decrypt(int key, const unsigned char* src, int srcLen,
                    unsigned char* dst, int* dstLen)
{
    // [N7 封顶] 同 Encrypt：m_graphs[type]->Decrypt 虚调用寄存器互换。
    if (srcLen < 1)
    {
        *dstLen = 0;
        return 1;
    }
    int type = GetCryptoType(key);
    int bs = m_graphs[type]->GetBlockSize();
    (void)bs;   // ORIG Decrypt 调 GetBlockSize 但不调整长度
    if (m_graphs[type]->Decrypt(src, srcLen, dst, *dstLen) == 0x6fffffff)
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
    // ORIG 0x82a713a（6 insns）：尾调 GlobalInstance<Cipher>::inst_ptr()（0x82ab9da），
    // 非 Meyers static。返回 GlobalInstance<T>::inst_ptr()→create()->m_p。
    return GlobalInstance<Cipher>::inst_ptr();
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
        // ORIG 对 rand() 结果做 `sarl$31;shrl$24;addl;andl$255;subl` 非负
        // mod256 归一（P176 族）——显式 `% 256` 触发该 codegen；rand 非负，
        // 语义与 (unsigned char) 相同。
        out[i] = (unsigned char)(std::rand() % 256);
}
