#ifndef METHOD_H_
#define METHOD_H_

class IMethod
{
public:
    IMethod()
    {
    }
    virtual ~IMethod()
    {
    }
    virtual void Signature(char* pcSig)
    {
    }

protected:
    bool m_bInit;
    int m_blockSize;
    int m_keylength;
    int m_iMode;
    int m_iPadding;
};

class CTEA : public IMethod
{
public:
    CTEA()
    {
    }
    virtual ~CTEA()
    {
    }

    static const char* sm_chain0;

    // [2026-09-14 修复] 与权威定义合并：ORIG df_bridge_r 里 `gCTEA` 的大小是
    //   **0x48**（vptr 4 + IMethod 20 + 下面这四个成员 0x30 = 72 = 0x48），
    //   说明 ORIG 用的就是 `DNFServer/GameServer/DBMW/TEA.h` 那份 CTEA ——
    //   ORIG df_bridge_r 导出的 `CTEA::Initialize(char const*, int, char const*,
    //   int, int)` / `CTEA::EncryptBlock(unsigned char const*, unsigned char*)`
    //   签名与它逐字一致。本地原先把 CTEA 写成一个**没有任何成员**的桩
    //   ⇒ gCTEA 只有 0x18 字节（由 global_size_audit.py 扫出）。
    //   注：`gCTEA` 在 ORIG 与本地**都没有被引用**（ORIG 反汇编里除
    //   static-init/析构之外 0 处引用），所以这一处只影响 .bss 布局、不影响行为；
    //   补齐成员是为了让对象大小与 ORIG 对齐。
    //   仍未还原：CTEA 的 Initialize/EncryptBlock/DecryptBlock/Encrypt/Decrypt/
    //   Signature/ResetChain 实现（属通用加密算法，按既定口径豁免）。
    unsigned char m_key[0x10];
    unsigned char m_iv[0x10];
    unsigned char m_chain[8];
    unsigned char m_chain2[8];
};

#endif // METHOD_H_
