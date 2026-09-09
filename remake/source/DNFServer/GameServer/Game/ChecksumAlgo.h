#ifndef DNF_GAME_CHECKSUM_ALGO_H_
#define DNF_GAME_CHECKSUM_ALGO_H_

// Batch01_ChecksumAlgo —— 校验和算法域权威声明（Game/W8LT 新 TU）。
// ORIG 符号地址证据见 ChecksumAlgo.cpp 文件头注释。
// 布局按 df_game_r 反汇编推导（标 [推断] 处为语义命名推断）。

enum CRYPTO_CHECKSUM_TYPE
{
    CHECKSUM_TYPE_MD5    = 0,   // ORIG CreateCryptoChecksum case 0 -> CNChecksumMd5
    CHECKSUM_TYPE_SHA256 = 1,   // case 1 -> CNChecksumSha256
    CHECKSUM_TYPE_SHA384 = 2,   // case 2 -> CNChecksumSha384
    CHECKSUM_TYPE_SHA512 = 3,   // case 3 -> CNChecksumSha512
};

// ORIG vtable @0x8b13bc8：slot0=D1 slot1=D0 slot2..8 纯虚（GetChecksumSize 位于
// vtable+0x20，即最后一个纯虚槽），对象布局：vptr@0x00 + int m_type@0x04。
class IChecksum
{
public:
    virtual ~IChecksum();

    // 声明顺序 = ORIG vtable 槽序（check_vtables.py 逐槽实证，_ZTV13CNChecksumMd5
    // @ORIG：[2]Initialize [3]MakeChecksumFromMemory [4]MakeChecksumFromFile
    // [5]ChecksumStart [6]ChecksumUpdate [7]ChecksumFinish [8]GetChecksumSize；
    // 派生类槽全为 override ⇒ 序由基类声明决定，勿重排）。
    virtual int  Initialize(unsigned char const* key, int keyLen) = 0;
    virtual int  MakeChecksumFromMemory(unsigned char const* data, int len,
                                        unsigned char* out, int outSize) = 0;
    virtual int  MakeChecksumFromFile(char const* path, unsigned char* out, int outSize) = 0;
    virtual int  ChecksumStart() = 0;
    virtual int  ChecksumUpdate(unsigned char const* data, int len) = 0;
    virtual int  ChecksumFinish(unsigned char* out, int outSize) = 0;
    virtual int  GetChecksumSize() const = 0;

protected:
    explicit IChecksum(CRYPTO_CHECKSUM_TYPE type);
    explicit IChecksum(IChecksum const& other);
    IChecksum& operator=(IChecksum const& other);

private:
    int m_type;   // @0x04，ORIG ctor 直接存参数
};

// ---- 前向声明：SHA 自由函数由各自 TU 提供（SHA256_* 见兄弟批次） ----
struct SHA256_ALG_INFO;
struct SHA384_ALG_INFO;
struct SHA512_ALG_INFO;

void SHA256_Init(SHA256_ALG_INFO*);
void SHA256_Update(SHA256_ALG_INFO*, unsigned char*, unsigned long);
void SHA256_Final(SHA256_ALG_INFO*, unsigned char*);

void SHA384_Init(SHA384_ALG_INFO*);
void SHA384_Update(SHA384_ALG_INFO*, unsigned char*, unsigned long);
void SHA384_Final(SHA384_ALG_INFO*, unsigned char*);

void SHA512_Init(SHA512_ALG_INFO*);
void SHA512_Update(SHA512_ALG_INFO*, unsigned char*, unsigned long);
void SHA512_Final(SHA512_ALG_INFO*, unsigned char*);

// 返回码（ORIG 各方法中出现的常量）
static const int CKS_OK           = (int)0x6fffffff;   // 0x6fffffff 成功
static const int CKS_ERR_NOSTATE  = (int)0x70000000;   // m_ctx == NULL
static const int CKS_ERR_BUFSIZE  = (int)0x70000001;   // 输出缓冲区小于摘要长度
static const int CKS_ERR_OPENFILE = (int)0x70000002;   // fopen 失败
static const int CKS_ERR_FILESIZE = (int)0x70000003;   // 路径为空 / ftell < 0
static const int CKS_ERR_DATALEN  = (int)0x70000005;   // MakeChecksumFromMemory len <= 0
static const int CKS_ERR_ADDLEN   = (int)0x70000007;   // CSHA::AddData len <= 0
static const int CKS_ERR_PENDING  = (int)0x70000008;   // CSHA::FinalDigest 缓冲区有残留数据

// ---- md5 wrapper：ctx 为 new 出的 0xd8 字节块（md5.cpp 的 md5_ctx 视图兼容） ----
class CNChecksumMd5 : public IChecksum
{
public:
    CNChecksumMd5();
    ~CNChecksumMd5();

    int  ChecksumStart();
    int  ChecksumUpdate(unsigned char const* data, int len);
    int  ChecksumFinish(unsigned char* out, int outSize);
    int  MakeChecksumFromFile(char const* path, unsigned char* out, int outSize);
    int  MakeChecksumFromMemory(unsigned char const* data, int len,
                                unsigned char* out, int outSize);
    int  Initialize(unsigned char const* key, int keyLen);
    int  GetChecksumSize() const;

private:
    unsigned char* m_ctx;   // @0x08；ctor 中 operator new(0xd8)
};

// ---- sha 族 wrapper：ctx 分别为 0x70 / 0xd0 / 0xd0 字节块 ----
class CNChecksumSha256 : public IChecksum
{
public:
    CNChecksumSha256();
    explicit CNChecksumSha256(CNChecksumSha256 const& other);
    ~CNChecksumSha256();
    CNChecksumSha256& operator=(CNChecksumSha256 const& other);   // ORIG：空操作返回 *this

    int  ChecksumStart();
    int  ChecksumUpdate(unsigned char const* data, int len);
    int  ChecksumFinish(unsigned char* out, int outSize);
    int  MakeChecksumFromFile(char const* path, unsigned char* out, int outSize);
    int  MakeChecksumFromMemory(unsigned char const* data, int len,
                                unsigned char* out, int outSize);
    int  Initialize(unsigned char const* key, int keyLen);
    int  GetChecksumSize() const;

private:
    unsigned char* m_ctx;   // @0x08；operator new(0x70)
};

class CNChecksumSha384 : public IChecksum
{
public:
    CNChecksumSha384();
    explicit CNChecksumSha384(CNChecksumSha384 const& other);
    ~CNChecksumSha384();
    CNChecksumSha384& operator=(CNChecksumSha384 const& other);
    int  ChecksumStart();
    int  ChecksumUpdate(unsigned char const* data, int len);
    int  ChecksumFinish(unsigned char* out, int outSize);
    int  MakeChecksumFromFile(char const* path, unsigned char* out, int outSize);
    int  MakeChecksumFromMemory(unsigned char const* data, int len,
                                unsigned char* out, int outSize);
    int  Initialize(unsigned char const* key, int keyLen);
    int  GetChecksumSize() const;

private:
    unsigned char* m_ctx;   // @0x08；operator new(0xd0)
};

class CNChecksumSha512 : public IChecksum
{
public:
    CNChecksumSha512();
    explicit CNChecksumSha512(CNChecksumSha512 const& other);
    ~CNChecksumSha512();
    CNChecksumSha512& operator=(CNChecksumSha512 const& other);

    int  ChecksumStart();
    int  ChecksumUpdate(unsigned char const* data, int len);
    int  ChecksumFinish(unsigned char* out, int outSize);
    int  MakeChecksumFromFile(char const* path, unsigned char* out, int outSize);
    int  MakeChecksumFromMemory(unsigned char const* data, int len,
                                unsigned char* out, int outSize);
    int  Initialize(unsigned char const* key, int keyLen);
    int  GetChecksumSize() const;

private:
    unsigned char* m_ctx;   // @0x08；operator new(0xd0)
};

// ---- CSHA：自实现 SHA-256（state[8]@0, bitLen lo@0x20 hi@0x24,
//      buffer[64]@0x28, pending flag byte@0x68），sizeof >= 0x69 ----
class CSHA
{
public:
    CSHA();

    int  AddData(char const* data, int len);            // ORIG _ZN4CSHA7AddDataEPKci
    int  FinalDigest(char* out);                        // ORIG _ZN4CSHA11FinalDigestEPc，输出 32 字节大端摘要
    void Reset();
    void Transform();                                    // 无参成员：消化 m_buffer[64]
    static unsigned int CHE(unsigned int x, unsigned int y, unsigned int z);
    static unsigned int MAJ(unsigned int x, unsigned int y, unsigned int z);
    static unsigned int SIG0(unsigned int x);
    static unsigned int SIG1(unsigned int x);
    static unsigned int sig0(unsigned int x);
    static unsigned int sig1(unsigned int x);

private:
    unsigned int     m_state[8];     // @0x00
    unsigned int     m_lenLo;        // @0x20 位长度低字
    unsigned int     m_lenHi;        // @0x24 位长度高字
    unsigned char    m_buffer[64];   // @0x28
    unsigned char    m_pending;      // @0x68：AddData 尾部残留标志（ORIG 行为照抄）

public:
    static void Word2Bytes(unsigned int const& word, unsigned char* out);  // 大端写出
    static void Bytes2Word(unsigned char const* in, unsigned int& word);   // 大端读入
};

// ---- CSHA1：经典 SHA-1 类 ----
// LinkHelper 核销批次修正：GetHexHash ORIG 为非 const（_ZN5CSHA110GetHexHashEPc）、
// Transform ORIG 为成员函数（_ZN5CSHA19TransformEPjPKh，经 m_blockPtr 使用 this）。
// 布局补证（报告 CSHA1.md）：m_blockPtr@0xc0 存 this+0x80，
// 即 0x80..0xc0 为 64 字节工作块区，补 m_pad74/m_work 具名字段（sizeof 0xc4 不变）。
class CSHA1
{
public:
    CSHA1();
    ~CSHA1();

    void Reset();
    void Update(unsigned char const* data, unsigned int len);
    void Final();
    int  GetHash(unsigned char* out) const;       // ORIG _ZNK5CSHA17GetHashEPh
    int  GetHexHash(char* out);                   // ORIG _ZN5CSHA110GetHexHashEPc
    void Transform(unsigned int* state, unsigned char const* block);

private:
    unsigned int    m_state[5];    // @0x00
    unsigned int    m_countLo;     // @0x14 已处理位数低字
    unsigned int    m_countHi;     // @0x18 高字
    unsigned char   m_buffer[64];  // @0x20
    unsigned char   m_digest[20];  // @0x60
    unsigned char   m_pad74[0xc];  // @0x74..0x7f（ORIG 未使用区）
    unsigned char   m_work[64];    // @0x80（m_blockPtr 指向区，Transform 展开块）
    unsigned char*  m_blockPtr;    // @0xc0（ORIG ctor 存 this+0x80）
};

// 全局工厂（ORIG 自由函数）
IChecksum*  CreateCryptoChecksum(CRYPTO_CHECKSUM_TYPE type);
void        DestroyCryptoChecksum(IChecksum*& p);

#endif  // DNF_GAME_CHECKSUM_ALGO_H_
