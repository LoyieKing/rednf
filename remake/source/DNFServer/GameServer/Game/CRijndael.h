#ifndef GAME_CRIJNDAEL_H_
#define GAME_CRIJNDAEL_H_

// df_game_r CRijndael（G7 AES/Rijndael 底层，2026-08-16 还原）。
// 依据 docs/class_func_reports/CRijndael.md 与 ORIG 二进制：
//   - 块大小 16/24/32（Nb=4/6/8），密钥长度 16/24/32（Nk=4/6/8）的完整
//     Rijndael（AES 为块 16 特例）；
//   - Initialize 把 key 按 keyLen 循环填充到 chainLen 字节作为实际密钥，
//     m_keyLength=chainLen、m_blockSize=blockSize（与 ORIG 一致）；
//   - 模式 0=ECB / 1=CBC / 2=CFB，chain 初始值取自 Initialize 的 IV 参数。
// 算法正确即可（用户规则 2026-08-16），不要求与 ORIG 逐指令 identical。

#include "CEnvironment.h"   // IMethod（G7 分组密码模式抽象基类）

class CRijndael : public IMethod
{
public:
    CRijndael();
    virtual ~CRijndael();

    // ---- IMethod 覆写 ----
    virtual void Signature(char* szSignature);
    virtual int Encrypt(const char* szIn, char* szOut, size_t iLength);
    virtual int Decrypt(const char* szIn, char* szOut, size_t iLength);
    virtual void ResetChain();

    // ---- 初始化（与 ORIG 签名一致）----
    int Initialize(const char* key, int keyLen, const char* chain, int chainLen,
                   int blockSize, int mode, int padding);

    // ---- 块处理（算法正确即可）----
    int EncryptBlock(const char* in, char* out);
    int DecryptBlock(const char* in, char* out);
    int DefEncryptBlock(const char* in, char* out);   // 块 16 特例（与 ORIG 同名）
    int DefDecryptBlock(const char* in, char* out);

    // ORIG 全零 16 字节初始链（0x8b22f20）
    static const char sm_chain0[0x10];

private:
    void KeyExpansion();
    void EncryptCore(const unsigned char* in, unsigned char* out);
    void DecryptCore(const unsigned char* in, unsigned char* out);
    void AddRoundKey(unsigned char state[4][8], int round);
    void InvAddRoundKey(unsigned char state[4][8], int round);

    // ---- ORIG 布局（本实现只保证接口正确，内部按需使用）----
    unsigned char m_key[0x20];      // +0x18 实际密钥（cycled）
    unsigned int  m_w[0x78];        // +0x38 加密轮密钥（最大 (14+1)*8 words）
    unsigned int  m_wd[0x78];       // +0x1f8 解密轮密钥（保留，本实现直接逆序用 m_w）
    unsigned int  m_rounds;         // +0x3d8
    unsigned char m_block[0x20];    // +0x3dc 初始 IV
    unsigned char m_chain[0x20];    // +0x3fc 当前链
    unsigned int  m_temp[8];        // +0x41c
    unsigned int  m_temp2[8];       // +0x43c
    unsigned int  m_temp3[8];       // +0x45c
};

#endif  // GAME_CRIJNDAEL_H_
