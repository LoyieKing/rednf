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
    : ICryptoGraph(CRYPTO_GRAPH_SHIFT)
{
    m_shift = 7;
    m_key = 0x1d3b4e9cU;
}

#include "CNCrypto/dtor_CNCryptoShift.inc"
{
}

#include "CNCrypto/CNCryptoShift_1.inc"
    : ICryptoGraph(CRYPTO_GRAPH_SHIFT)
{
}

CNCryptoShift& CNCryptoShift::operator=(const CNCryptoShift& other)
{
    return *this;
}

#include "CNCrypto/SetMode.inc"
{
    return 0x70000017;
}

#include "CNCrypto/Initialize.inc"
{
    if (keyLen < 8)
        return 0;
    // ORIG 0x0809c4fe：32 位直读 + 形参自增（addl $0x4,0xc(%ebp)）。
    m_shift = *(const unsigned int*)key & 0x1f;
    key += 4;
    m_key = *(const unsigned int*)key;
    return 0x6fffffff;
}

#include "CNCrypto/Encrypt.inc"
{
    if (m_shift >= 0x20)
        return 0x7000000c;
    int blockSize = GetBlockSize();
    if (len % blockSize != 0)
        return 0x70000006;
    unsigned int* p = (unsigned int*)buf;
    int count = len / GetBlockSize();
    for (int i = 0; i < count; ++i)
    {
        unsigned int v = *p;
        ROR(v, (int)m_shift);   // ORIG 丢弃返回值（原码遗留，P171 照抄）
        v ^= m_key;
        *p = v;
        ++p;
    }
    return 0x6fffffff;
}

#include "CNCrypto/Decrypt.inc"
{
    if (m_shift >= 0x20)
        return 0x7000000c;
    int blockSize = GetBlockSize();
    if (len % blockSize != 0)
        return 0x70000006;
    unsigned int* p = (unsigned int*)buf;
    int count = len / GetBlockSize();
    for (int i = 0; i < count; ++i)
    {
        unsigned int v = *p;
        v ^= m_key;
        ROL(v, (int)m_shift);   // ORIG 丢弃返回值（原码遗留，P171 照抄）
        *p = v;
        ++p;
    }
    return 0x6fffffff;
}

#include "CNCrypto/Encrypt_1.inc"
                           unsigned char* out, int outCap)
{
    if (m_shift >= 0x20)
        return 0x7000000c;
    int blockSize = GetBlockSize();
    if (inLen % blockSize != 0)
        return 0x70000006;
    if (inLen > outCap)
        return 0x7000000a;
    const unsigned int* src = (const unsigned int*)in;
    unsigned int* dst = (unsigned int*)out;
    int count = inLen / GetBlockSize();
    for (int i = 0; i < count; ++i)
    {
        unsigned int v = *src;
        ++src;
        ROR(v, (int)m_shift);   // ORIG 丢弃返回值（原码遗留，P171 照抄）
        v ^= m_key;
        *dst = v;
        ++dst;
    }
    return 0x6fffffff;
}

#include "CNCrypto/Decrypt_1.inc"
                           unsigned char* out, int outCap)
{
    if (m_shift >= 0x20)
        return 0x7000000c;
    int blockSize = GetBlockSize();
    if (inLen % blockSize != 0)
        return 0x70000006;
    if (inLen > outCap)
        return 0x7000000a;
    const unsigned int* src = (const unsigned int*)in;
    unsigned int* dst = (unsigned int*)out;
    int count = inLen / GetBlockSize();
    for (int i = 0; i < count; ++i)
    {
        unsigned int v = *src;
        ++src;
        v ^= m_key;
        ROL(v, (int)m_shift);   // ORIG 丢弃返回值（原码遗留，P171 照抄）
        *dst = v;
        ++dst;
    }
    return 0x6fffffff;
}

#include "CNCrypto/GetKeySize.inc"
{
    return 8;
}

#include "CNCrypto/GetBlockSize.inc"
{
    return 4;
}

// ============================================================================
// CNCryptoRijndael（AES/Rijndael 封装：块 16 / 钥 16）
// ============================================================================

#include "CNCrypto/CNCryptoRijndael.inc"
    : ICryptoGraph(CRYPTO_GRAPH_RIJNDAEL)
{
    m_pCrypto = 0;
    m_pCrypto = new CRijndael;
    if (m_pCrypto != 0)
    {
        char key[0x80];
        GenerateRandomString(key, 0x80);
        if (((CRijndael*)m_pCrypto)->Initialize(key, 0x80, CRijndael::sm_chain0,
                                                0x10, 0x10, 0, 0) != 0x6fffffff)
        {
            delete (CRijndael*)m_pCrypto;
            m_pCrypto = 0;
        }
    }
}

#include "CNCrypto/dtor_CNCryptoRijndael.inc"
{
    if (m_pCrypto)
    {
        delete (CRijndael*)m_pCrypto;
        m_pCrypto = 0;
    }
}

#include "CNCrypto/CNCryptoRijndael_1.inc"
    : ICryptoGraph(CRYPTO_GRAPH_RIJNDAEL)
{
    m_pCrypto = 0;
}

CNCryptoRijndael& CNCryptoRijndael::operator=(const CNCryptoRijndael& other)
{
    return *this;
}

#include "CNCrypto/SetMode_1.inc"
{
    if (!m_pCrypto)
        return 0x7000000c;
    // ORIG 0x0809c129..0x0809c187（逐分支内联立即数实参、无中间槽；三 call 后
    // 直接 jmp leave ⇒ **返回值 = IMethod::SetMode 的 eax，非固定码**；cmp 序
    // 1,3,0；其余 mode ⇒ 0x70000017）：
    if (mode == 1)
        return ((IMethod*)m_pCrypto)->SetMode(1);
    if (mode == 3)
        return ((IMethod*)m_pCrypto)->SetMode(2);
    if (mode == 0)
        return ((IMethod*)m_pCrypto)->SetMode(0);
    return 0x70000017;
}

#include "CNCrypto/Initialize_1.inc"
{
    if (!m_pCrypto)
        return 0x7000000c;
    return ((CRijndael*)m_pCrypto)->Initialize(
        (const char*)key, keyLen, CRijndael::sm_chain0, 0x10, 0x10, 0, 0);
}

#include "CNCrypto/Encrypt_2.inc"
{
    if (!m_pCrypto)
        return 0x7000000c;
    int blockSize = GetBlockSize();
    if (len % blockSize != 0)
        return 0x70000006;
    return ((CRijndael*)m_pCrypto)->Encrypt((const char*)buf, (char*)buf, len);
}

#include "CNCrypto/Decrypt_2.inc"
{
    if (!m_pCrypto)
        return 0x7000000c;
    int blockSize = GetBlockSize();
    if (len % blockSize != 0)
        return 0x70000006;
    return ((CRijndael*)m_pCrypto)->Decrypt((const char*)buf, (char*)buf, len);
}

#include "CNCrypto/Encrypt_3.inc"
                              unsigned char* out, int outCap)
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (inLen > outCap)
        return 0x7000000b;
    int blockSize = GetBlockSize();
    if (inLen % blockSize != 0)
        return 0x70000006;
    return ((CRijndael*)m_pCrypto)->Encrypt((const char*)in, (char*)out, inLen);
}

#include "CNCrypto/Decrypt_3.inc"
                              unsigned char* out, int outCap)
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (inLen > outCap)
        return 0x7000000b;
    int blockSize = GetBlockSize();
    if (inLen % blockSize != 0)
        return 0x70000006;
    return ((CRijndael*)m_pCrypto)->Decrypt((const char*)in, (char*)out, inLen);
}

#include "CNCrypto/GetKeySize_1.inc"
{
    return 0x10;
}

#include "CNCrypto/GetBlockSize_1.inc"
{
    return 0x10;
}

// ============================================================================
// CNCryptoTea（TEA 封装：块 16 / 钥 16，底层 CTEA）
// ============================================================================

#include "CNCrypto/CNCryptoTea.inc"
    : ICryptoGraph(CRYPTO_GRAPH_TEA)
{
    m_pCrypto = 0;
    m_pCrypto = new CTEA;
    if (m_pCrypto != 0)
    {
        char key[0x80];
        GenerateRandomString(key, 0x80);
        if (((CTEA*)m_pCrypto)->Initialize(key, 0x80, "", 0, 0) != 0x6fffffff)
        {
            delete (CTEA*)m_pCrypto;
            m_pCrypto = 0;
        }
    }
}

#include "CNCrypto/dtor_CNCryptoTea.inc"
{
    if (m_pCrypto)
    {
        delete (CTEA*)m_pCrypto;
        m_pCrypto = 0;
    }
}

#include "CNCrypto/CNCryptoTea_1.inc"
    : ICryptoGraph(CRYPTO_GRAPH_TEA)
{
    m_pCrypto = 0;
}

CNCryptoTea& CNCryptoTea::operator=(const CNCryptoTea& other)
{
    return *this;
}

#include "CNCrypto/SetMode_2.inc"
{
    if (!m_pCrypto)
        return 0x7000000c;
    // ORIG 0x0809d061..0x0809d0bf：与 Rijndael 同构（call IMethod::SetMode、
    // 返回值透传；cmp 序 1,3,0），但**未匹配失败码 = 0x70000003**（0x0809d0ba
    // `mov $0x70000003,%eax`；旧形直写 CTEA::m_mode + 固定 0x6fffffff 属误转写）。
    if (mode == 1)
        return ((IMethod*)m_pCrypto)->SetMode(1);
    if (mode == 3)
        return ((IMethod*)m_pCrypto)->SetMode(2);
    if (mode == 0)
        return ((IMethod*)m_pCrypto)->SetMode(0);
    return 0x70000003;
}

#include "CNCrypto/Initialize_2.inc"
{
    if (!m_pCrypto)
        return 0x7000000c;
    return ((CTEA*)m_pCrypto)->Initialize((const char*)key, keyLen, "", 0, 0);
}

#include "CNCrypto/Encrypt_4.inc"
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (len < 1)
        return 0x7000000b;
    int blockSize = GetBlockSize();
    if (len % blockSize != 0)
        return 0x70000006;
    return ((CTEA*)m_pCrypto)->Encrypt((const char*)buf, (char*)buf, len);
}

#include "CNCrypto/Decrypt_4.inc"
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (len < 1)
        return 0x7000000b;
    int blockSize = GetBlockSize();
    if (len % blockSize != 0)
        return 0x70000006;
    return ((CTEA*)m_pCrypto)->Decrypt((const char*)buf, (char*)buf, len);
}

#include "CNCrypto/Encrypt_5.inc"
                         unsigned char* out, int outCap)
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (inLen > outCap)
        return 0x7000000b;
    int blockSize = GetBlockSize();
    if (inLen % blockSize != 0)
        return 0x70000006;
    return ((CTEA*)m_pCrypto)->Encrypt((const char*)in, (char*)out, inLen);
}

#include "CNCrypto/Decrypt_5.inc"
                         unsigned char* out, int outCap)
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (inLen > outCap)
        return 0x7000000b;
    // ORIG 0x0809d2b6：4 参 Decrypt 无 GetBlockSize/模检查，直接虚调用 +0x10。
    return ((CTEA*)m_pCrypto)->Decrypt((const char*)in, (char*)out, inLen);
}

#include "CNCrypto/GetKeySize_2.inc"
{
    return 0x10;
}

#include "CNCrypto/GetBlockSize_2.inc"
{
    return 0x10;
}

// ============================================================================
// CNCryptoBlowFish（Blowfish 封装：块 8 / 钥 56）
// ============================================================================

#include "CNCrypto/CNCryptoBlowFish.inc"
    : ICryptoGraph(CRYPTO_GRAPH_BLOWFISH)
{
    m_pCrypto = 0;
    m_mode = 0;
    // ORIG 0x0809980c..0x080998f7：GenerateRandomKey(key,0x38) → SBlock chain(0,0)
    // → new CBlowFish(key,0x38,chain) → m_pCrypto=c；m_pCrypto==0 时
    // NCryptoSetLastError(0x7000000d)（0x0809e45c）。
    unsigned char key[0x38];
    GenerateRandomKey(key, 0x38);
    SBlock chain(0, 0);
    CBlowFish* c = new CBlowFish(key, 0x38, chain);
    m_pCrypto = c;
    if (m_pCrypto == 0)
        NCryptoSetLastError(0x7000000d);
}

#include "CNCrypto/dtor_CNCryptoBlowFish.inc"
{
    if (m_pCrypto)
    {
        delete (symmetric_key*)m_pCrypto;
        m_pCrypto = 0;
    }
}

#include "CNCrypto/CNCryptoBlowFish_1.inc"
    : ICryptoGraph(CRYPTO_GRAPH_BLOWFISH)
{
}

CNCryptoBlowFish& CNCryptoBlowFish::operator=(const CNCryptoBlowFish& other)
{
    return *this;
}

#include "CNCrypto/SetMode_3.inc"
{
    m_mode = (unsigned int)mode;
    return 0x6fffffff;
}

#include "CNCrypto/Initialize_3.inc"
{
    // ORIG 0x080999a8..0x08099b29：按 keyLen 两路 new CBlowFish（>=0x38 用原 key，
    // <0x38 先 memset 0x38 + memcpy keyLen 补零），成功后 ::operator delete 旧指针
    // 并换入新对象；new 失败返回 0x7000000d，成功 0x6fffffff。
    if (keyLen < 1)
        return 0x70000002;
    if (keyLen > 0x37)
    {
        SBlock chain(0, 0);
        CBlowFish* c = new CBlowFish((unsigned char*)key, 0x38, chain);
        if (c != 0)
        {
            ::operator delete(m_pCrypto);
            m_pCrypto = c;
        }
        else
        {
            return 0x7000000d;
        }
    }
    else
    {
        unsigned char buf[0x38];
        std::memset(buf, 0, 0x38);
        std::memcpy(buf, key, keyLen);
        SBlock chain(0, 0);
        CBlowFish* c = new CBlowFish(buf, 0x38, chain);
        if (c != 0)
        {
            ::operator delete(m_pCrypto);
            m_pCrypto = c;
        }
        else
        {
            return 0x7000000d;
        }
    }
    return 0x6fffffff;
}

#include "CNCrypto/Encrypt_6.inc"
{
    if (!m_pCrypto)
        return 0x7000000c;
    int blockSize = GetBlockSize();
    if (len % blockSize != 0)
        return 0x70000006;
    // ORIG 0x08099b2a：按 m_mode 分派到 CBlowFish::Encrypt(in,out,len,mode)；
    // mode 1 与 3 同传 1，mode 0 传 0，其余 0x70000017。
    if (m_mode == 1)
        return ((CBlowFish*)m_pCrypto)->Encrypt(buf, buf, len, 1);
    if (m_mode == 3)
        return ((CBlowFish*)m_pCrypto)->Encrypt(buf, buf, len, 2);   // ORIG 2 参版 mode3 传 2
    if (m_mode == 0)
        return ((CBlowFish*)m_pCrypto)->Encrypt(buf, buf, len, 0);
    return 0x70000017;
}

#include "CNCrypto/Decrypt_6.inc"
{
    if (!m_pCrypto)
        return 0x7000000c;
    int blockSize = GetBlockSize();
    if (len % blockSize != 0)
        return 0x70000006;
    if (m_mode == 1)
        return ((CBlowFish*)m_pCrypto)->Decrypt(buf, buf, len, 1);
    if (m_mode == 3)
        return ((CBlowFish*)m_pCrypto)->Decrypt(buf, buf, len, 2);   // ORIG 2 参版 mode3 传 2
    if (m_mode == 0)
        return ((CBlowFish*)m_pCrypto)->Decrypt(buf, buf, len, 0);
    return 0x70000017;
}

#include "CNCrypto/Encrypt_7.inc"
                              unsigned char* out, int outCap)
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (inLen > outCap)
        return 0x7000000a;
    if ((inLen & 7) != 0)
        return 0x70000006;
    if (m_mode == 1)
        return ((CBlowFish*)m_pCrypto)->Encrypt(in, out, inLen, 1);
    if (m_mode == 3)
        return ((CBlowFish*)m_pCrypto)->Encrypt(in, out, inLen, 1);
    if (m_mode == 0)
        return ((CBlowFish*)m_pCrypto)->Encrypt(in, out, inLen, 0);
    return 0x70000017;
}

#include "CNCrypto/Decrypt_7.inc"
                              unsigned char* out, int outCap)
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (inLen > outCap)
        return 0x7000000a;
    if ((inLen & 7) != 0)
        return 0x70000006;
    if (m_mode == 1)
        return ((CBlowFish*)m_pCrypto)->Decrypt(in, out, inLen, 1);
    if (m_mode == 3)
        return ((CBlowFish*)m_pCrypto)->Decrypt(in, out, inLen, 1);
    if (m_mode == 0)
        return ((CBlowFish*)m_pCrypto)->Decrypt(in, out, inLen, 0);
    return 0x70000017;
}

#include "CNCrypto/GetKeySize_3.inc"
{
    return 0x38;
}

#include "CNCrypto/GetBlockSize_3.inc"
{
    return 8;
}

// ============================================================================
// CNCryptoRc6（RC6 封装：块 16 / 钥 60；2 参数版 ORIG 直接返回不支持）
// ============================================================================

#include "CNCrypto/CNCryptoRc6.inc"
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

#include "CNCrypto/dtor_CNCryptoRc6.inc"
{
    if (m_pCrypto)
    {
        delete (RC6_ALG_INFO*)m_pCrypto;
        m_pCrypto = 0;
    }
}

#include "CNCrypto/CNCryptoRc6_1.inc"
    : ICryptoGraph(CRYPTO_GRAPH_RC6)
{
    m_pCrypto = 0;
}

CNCryptoRc6& CNCryptoRc6::operator=(const CNCryptoRc6& other)
{
    return *this;
}

#include "CNCrypto/SetMode_4.inc"
{
    return 0x70000017;
}

#include "CNCrypto/Initialize_4.inc"
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (keyLen < 0x3c)
        return 0x70000002;
    RC6_ALG_INFO* c = (RC6_ALG_INFO*)m_pCrypto;
    // ORIG 0x809bc84：memcpy(+0x0, key, 0x20) + memcpy(+0x20, key, 0x2c)
    // （K 区初始 = key 前 0x2c 字节）→ rc6_key_setup(c, 0x20)。
    std::memcpy(c->key, key, 0x20);
    std::memcpy(c->K, key, 0x2c);
    rc6_key_setup(c, 0x20);
    return 0x6fffffff;
}

#include "CNCrypto/Encrypt_8.inc"
{
    return 0x70000017;
}

#include "CNCrypto/Decrypt_8.inc"
{
    return 0x70000017;
}

#include "CNCrypto/Encrypt_9.inc"
                         unsigned char* out, int outCap)
{
    if (!m_pCrypto)
        return 0x7000000c;
    int blockSize = GetBlockSize();
    if (inLen % blockSize != 0)
        return 0x70000006;
    if (inLen > outCap)
        return 0x7000000a;
    int count = inLen / GetBlockSize();
    for (int i = 0; i < count; ++i)
    {
        rc6_block_encrypt((const RC6_ALG_INFO*)m_pCrypto,
                          (const unsigned int*)in, (unsigned int*)out);
        in += GetBlockSize();
        out += GetBlockSize();
    }
    return 0x6fffffff;
}

#include "CNCrypto/Decrypt_9.inc"
                         unsigned char* out, int outCap)
{
    if (!m_pCrypto)
        return 0x7000000c;
    int blockSize = GetBlockSize();
    if (inLen % blockSize != 0)
        return 0x70000006;
    if (inLen > outCap)
        return 0x7000000a;
    int count = inLen / GetBlockSize();
    for (int i = 0; i < count; ++i)
    {
        rc6_block_decrypt((const RC6_ALG_INFO*)m_pCrypto,
                          (const unsigned int*)in, (unsigned int*)out);
        in += GetBlockSize();
        out += GetBlockSize();
    }
    return 0x6fffffff;
}

#include "CNCrypto/GetKeySize_4.inc"
{
    return 0x3c;
}

#include "CNCrypto/GetBlockSize_4.inc"
{
    return 0x10;
}

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
