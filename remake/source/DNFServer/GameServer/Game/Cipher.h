#ifndef GAME_CIPHER_H_
#define GAME_CIPHER_H_

// df_game_r Cipher（G7 分组算法容器，2026-08-16 还原）。
// 依据 docs/class_func_reports/Cipher.md、ICryptoGraph.md 与 ORIG 二进制：
//   - ICryptoGraph 虚表：+0x0c Initialize、+0x18 Encrypt4、+0x1c Decrypt4、
//     +0x20 GetKeySize、+0x24 GetBlockSize（+0x08 SetMode，纯虚）；
//   - Cipher 持有 14 个 ICryptoGraph（Anubis…XTea），按 key % 14 索引，
//     Encrypt 前按块大小向上取整；Initialize 把总钥（GetTotalKeyLength 字节）
//     顺序分发给各图。

#include <vector>

enum CRYPTO_GRAPH_TYPE
{
    CRYPTO_GRAPH_SHIFT = 0,
    CRYPTO_GRAPH_RIJNDAEL,
    CRYPTO_GRAPH_BLOWFISH,
    CRYPTO_GRAPH_RC6,
    CRYPTO_GRAPH_TWOFISH,
    CRYPTO_GRAPH_TEA,
    CRYPTO_GRAPH_KASUMI,
    CRYPTO_GRAPH_XTEA,
    CRYPTO_GRAPH_NOEKEON,
    CRYPTO_GRAPH_KHAZAD,
    CRYPTO_GRAPH_CAST5,
    CRYPTO_GRAPH_SKIPJACK,
    CRYPTO_GRAPH_MULTI2,
    CRYPTO_GRAPH_ANUBIS,
    CRYPTO_GRAPH_MAX
};

class ICryptoGraph
{
public:
    ICryptoGraph(CRYPTO_GRAPH_TYPE type);
    ICryptoGraph(const ICryptoGraph& other);
    virtual ~ICryptoGraph();
    ICryptoGraph& operator=(const ICryptoGraph& other);

    // 虚表槽位（与 ORIG 一致）：
    //   +0x00 D1  +0x04 D0  +0x08 SetMode  +0x0c Initialize
    //   +0x10 Encrypt2  +0x14 Decrypt2  +0x18 Encrypt4  +0x1c Decrypt4
    //   +0x20 GetKeySize  +0x24 GetBlockSize
    virtual int SetMode(int mode) = 0;
    virtual int Initialize(const unsigned char* key, int keyLen) = 0;
    virtual int Encrypt(unsigned char* buf, int len) = 0;
    virtual int Decrypt(unsigned char* buf, int len) = 0;
    virtual int Encrypt(const unsigned char* in, int inLen,
                        unsigned char* out, int outCap) = 0;
    virtual int Decrypt(const unsigned char* in, int inLen,
                        unsigned char* out, int outCap) = 0;
    virtual int GetKeySize() const = 0;
    virtual int GetBlockSize() const = 0;

    int GetApiType() const;

protected:
    unsigned int m_apiType;   // +0x04
};

class Cipher
{
public:
    Cipher();
    ~Cipher();
    Cipher(const Cipher& other);          // ORIG 拷贝构造只建空 vector
    Cipher& operator=(const Cipher& other);

    int GetTotalKeyLength() const;
    int Initialize(const unsigned char* key, int keyLen);
    void GetOriginalKey(unsigned char* dst) const;
    void AdjustDataSize(int key, int* size) const;
    int Encrypt(int key, const unsigned char* src, int srcLen,
                unsigned char* dst, int* dstLen) const;
    int Decrypt(int key, const unsigned char* src, int srcLen,
                unsigned char* dst, int* dstLen) const;

private:
    int m_totalKeyLength;                 // +0x00
    std::vector<ICryptoGraph*> m_graphs;  // +0x04
    unsigned char m_keyBuf[0x400];        // +0x10 原始总钥
};

// ---- 工厂 / 全局 ----
int GetCryptoType(int key);
ICryptoGraph* CreateCryptoGraph(CRYPTO_GRAPH_TYPE type);
void DestroyCryptoGraph(ICryptoGraph*& graph);
Cipher* G_Cipher();

// ---- 随机串 / 随机钥（ORIG @0x809e473 / @0x809e6b2）----
void GenerateRandomString(char* out, int len);
void GenerateRandomKey(unsigned char* out, int len);

#endif  // GAME_CIPHER_H_
