// ============================================================================
// Wave9_Crypto —— 校验/AES 密码学族实现（单 TU）。
// 符号证据：df_game_r nm -S + docs/class_func_reports/{CNChecksumMd5,
// CNChecksumSha256,CNChecksumSha384,CNChecksumSha512,CRijndael_fake}。
// 用户规则：算法族为语义正确真实现（标准 FIPS 180-4 / FIPS-197 Rijndael），
// 不要求逐指令 identical；类封装结构与错误路径按 ORIG 报告转写。
// ORIG 错误码：CKS_* 见 ChecksumAlgo.h；AES 族 0x1001..0x1004 见头文件。
// ============================================================================

#include "ChecksumCrypto.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "CNCryptoAlgo.h"   // anubis_setup/anubis_ecb_encrypt/anubis_ecb_decrypt/symmetric_key
#include "md5.h"            // md5_starts/md5_update/md5_finish/md5_context

// ---------------------------------------------------------------------------
// IChecksum 生命周期（ORIG 0x0809de70..0x0809dee6）
// ---------------------------------------------------------------------------

IChecksum::IChecksum(CRYPTO_CHECKSUM_TYPE type)
    : m_type((int)type)
{
}

IChecksum::IChecksum(IChecksum const& other)
    : m_type(other.m_type)
{
}

IChecksum& IChecksum::operator=(IChecksum const& other)
{
    return *this;   // ORIG 0x0809dee6：仅返回 this
}

IChecksum::~IChecksum()
{
}

void DestroyCryptoChecksum(IChecksum*& p)
{
    if (p != 0) {
        delete p;
        p = 0;
    }
}

// ---------------------------------------------------------------------------
// 公共文件校验流程（ORIG 各 MakeChecksumFromFile 反编译同构）
// ---------------------------------------------------------------------------

namespace {

struct ShaFileHooks
{
    void (*init)(void*);
    void (*update)(void*, unsigned char const*, unsigned long);
    void (*finish)(void*, unsigned char*);
};

int MakeShaChecksumFromFile(void* ctx, ShaFileHooks const& hooks,
                            char const* path, unsigned char* out, int outSize,
                            int digestSize)
{
    if (ctx == 0)
        return CKS_ERR_NOSTATE;
    if (outSize < digestSize)
        return CKS_ERR_BUFSIZE;
    if (path == 0)
        return CKS_ERR_FILESIZE;

    FILE* fp = std::fopen(path, "rb");
    if (fp == 0)
        return CKS_ERR_OPENFILE;

    std::fseek(fp, 0, SEEK_END);
    long fileLen = std::ftell(fp);
    if (fileLen < 0) {
        std::fclose(fp);
        return CKS_ERR_FILESIZE;
    }
    std::fseek(fp, 0, SEEK_SET);

    unsigned char buf[1024];
    hooks.init(ctx);
    for (unsigned long off = 0; off < (unsigned long)fileLen; ) {
        size_t chunk = (unsigned long)fileLen - off;
        if (chunk > sizeof(buf))
            chunk = sizeof(buf);
        std::fread(buf, chunk, 1, fp);
        hooks.update(ctx, buf, (unsigned long)chunk);
        off += (unsigned long)chunk;
    }
    hooks.finish(ctx, out);
    std::fclose(fp);
    return CKS_OK;
}

int MakeShaChecksumFromMemory(void* ctx, ShaFileHooks const& hooks,
                              unsigned char const* data, int len,
                              unsigned char* out, int outSize, int digestSize)
{
    if (ctx == 0)
        return CKS_ERR_NOSTATE;
    if (outSize < digestSize)
        return CKS_ERR_BUFSIZE;
    hooks.init(ctx);
    hooks.update(ctx, data, (unsigned long)len);
    hooks.finish(ctx, out);
    return CKS_OK;
}

}  // namespace

// ---------------------------------------------------------------------------
// CNChecksumMd5（ORIG 0x0809830c..0x080986ac）
// ---------------------------------------------------------------------------

CNChecksumMd5::CNChecksumMd5()
    : IChecksum(CHECKSUM_TYPE_MD5),
      m_ctx(0)
{
    m_ctx = new unsigned char[0xd8];   // ORIG operator new(0xd8)：md5_context 视图块
}

CNChecksumMd5::~CNChecksumMd5()
{
    if (m_ctx != 0) {
        delete[] m_ctx;
        m_ctx = 0;
    }
}

int CNChecksumMd5::Initialize(unsigned char const* /*key*/, int /*keyLen*/)
{
    return CKS_ERR_DATALEN;   // ORIG 0x080983e0：恒返 0x70000017
}

int CNChecksumMd5::GetChecksumSize() const
{
    return 0x10;
}

int CNChecksumMd5::ChecksumStart()
{
    if (m_ctx == 0)
        return CKS_ERR_NOSTATE;
    md5_starts(reinterpret_cast<md5_context*>(m_ctx));
    return CKS_OK;
}

int CNChecksumMd5::ChecksumUpdate(unsigned char const* data, int len)
{
    if (m_ctx == 0)
        return CKS_ERR_NOSTATE;
    md5_update(reinterpret_cast<md5_context*>(m_ctx),
               const_cast<unsigned char*>(data), len);
    return CKS_OK;
}

int CNChecksumMd5::ChecksumFinish(unsigned char* out, int outSize)
{
    if (m_ctx == 0)
        return CKS_ERR_NOSTATE;
    if (outSize < GetChecksumSize())
        return CKS_ERR_BUFSIZE;
    md5_finish(reinterpret_cast<md5_context*>(m_ctx), out);
    return CKS_OK;
}

int CNChecksumMd5::MakeChecksumFromMemory(unsigned char const* data, int len,
                                          unsigned char* out, int outSize)
{
    if (m_ctx == 0)
        return CKS_ERR_NOSTATE;
    if (outSize < GetChecksumSize())
        return CKS_ERR_BUFSIZE;
    ChecksumStart();
    ChecksumUpdate(data, len);
    ChecksumFinish(out, outSize);
    return CKS_OK;
}

int CNChecksumMd5::MakeChecksumFromFile(char const* path, unsigned char* out,
                                        int outSize)
{
    if (m_ctx == 0)
        return CKS_ERR_NOSTATE;
    if (outSize < GetChecksumSize())
        return CKS_ERR_BUFSIZE;
    if (path == 0)
        return CKS_ERR_FILESIZE;

    FILE* fp = std::fopen(path, "rb");
    if (fp == 0)
        return CKS_ERR_OPENFILE;

    std::fseek(fp, 0, SEEK_END);
    long fileLen = std::ftell(fp);
    if (fileLen < 0) {
        std::fclose(fp);
        return CKS_ERR_FILESIZE;
    }
    std::fseek(fp, 0, SEEK_SET);

    unsigned char buf[1024];
    ChecksumStart();
    for (unsigned long off = 0; off < (unsigned long)fileLen; ) {
        size_t chunk = (unsigned long)fileLen - off;
        if (chunk > sizeof(buf))
            chunk = sizeof(buf);
        std::fread(buf, chunk, 1, fp);
        ChecksumUpdate(buf, (int)chunk);
        off += (unsigned long)chunk;
    }
    ChecksumFinish(out, outSize);
    std::fclose(fp);
    return CKS_OK;
}

// ---------------------------------------------------------------------------
// CNChecksumSha256/384/512（wrapper 结构与 ORIG 报告一致）
// ---------------------------------------------------------------------------


// hooks 前置声明（内核实现于文件后部）
static void sha256HookInit(void* c);
static void sha256HookUpdate(void* c, unsigned char const* d, unsigned long n);
static void sha256HookFinish(void* c, unsigned char* o);
static void sha384HookInit(void* c);
static void sha384HookUpdate(void* c, unsigned char const* d, unsigned long n);
static void sha384HookFinish(void* c, unsigned char* o);
static void sha512HookInit(void* c);
static void sha512HookUpdate(void* c, unsigned char const* d, unsigned long n);
static void sha512HookFinish(void* c, unsigned char* o);

static ShaFileHooks Hooks256()
{
    ShaFileHooks h;
    h.init = &sha256HookInit;
    h.update = &sha256HookUpdate;
    h.finish = &sha256HookFinish;
    return h;
}
static ShaFileHooks Hooks384()
{
    ShaFileHooks h;
    h.init = &sha384HookInit;
    h.update = &sha384HookUpdate;
    h.finish = &sha384HookFinish;
    return h;
}
static ShaFileHooks Hooks512()
{
    ShaFileHooks h;
    h.init = &sha512HookInit;
    h.update = &sha512HookUpdate;
    h.finish = &sha512HookFinish;
    return h;
}

#define DNF_SHA_BODY(CLASS, TYPE_ID, CTX_SIZE, DIGEST_SIZE)                    \
CLASS::CLASS()                                                                 \
    : IChecksum(TYPE_ID), m_ctx(0)                                             \
{                                                                              \
    m_ctx = new unsigned char[CTX_SIZE];                                       \
}                                                                              \
void CLASS::InitHooks()                                                        \
{                                                                              \
}                                                                              \
CLASS::~CLASS()                                                                \
{                                                                              \
    if (m_ctx != 0) {                                                          \
        delete[] m_ctx;                                                        \
        m_ctx = 0;                                                             \
    }                                                                          \
}
