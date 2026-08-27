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
// CNChecksumSha256 / Sha384 / Sha512（显式实现；copy ctor/or= 与 ORIG 一致）
// ---------------------------------------------------------------------------

static void sha256HookInit(void* c);
static void sha256HookUpdate(void* c, unsigned char const* d, unsigned long n);
static void sha256HookFinish(void* c, unsigned char* o);
static void sha384HookInit(void* c);
static void sha384HookUpdate(void* c, unsigned char const* d, unsigned long n);
static void sha384HookFinish(void* c, unsigned char* o);
static void sha512HookInit(void* c);
static void sha512HookUpdate(void* c, unsigned char const* d, unsigned long n);
static void sha512HookFinish(void* c, unsigned char* o);

namespace {

ShaFileHooks Hooks256()
{
    ShaFileHooks h;
    h.init = &sha256HookInit;
    h.update = &sha256HookUpdate;
    h.finish = &sha256HookFinish;
    return h;
}
ShaFileHooks Hooks384()
{
    ShaFileHooks h;
    h.init = &sha384HookInit;
    h.update = &sha384HookUpdate;
    h.finish = &sha384HookFinish;
    return h;
}
ShaFileHooks Hooks512()
{
    ShaFileHooks h;
    h.init = &sha512HookInit;
    h.update = &sha512HookUpdate;
    h.finish = &sha512HookFinish;
    return h;
}

}  // namespace

// ---- CNChecksumSha256（ORIG 0x080986b8..0x08098a9c）----

CNChecksumSha256::CNChecksumSha256()
    : IChecksum(CHECKSUM_TYPE_SHA256), m_ctx(0)
{
    m_ctx = new unsigned char[0x70];
}

CNChecksumSha256::CNChecksumSha256(CNChecksumSha256 const&)
    : IChecksum(CHECKSUM_TYPE_SHA256), m_ctx(0)
{
}

CNChecksumSha256::~CNChecksumSha256()
{
    if (m_ctx != 0) {
        delete[] m_ctx;
        m_ctx = 0;
    }
}

CNChecksumSha256& CNChecksumSha256::operator=(CNChecksumSha256 const&)
{
    return *this;   // ORIG：空操作
}

int CNChecksumSha256::Initialize(unsigned char const*, int)
{ return CKS_ERR_DATALEN; }

int CNChecksumSha256::GetChecksumSize() const
{ return 0x20; }

int CNChecksumSha256::ChecksumStart()
{
    if (m_ctx == 0)
        return CKS_ERR_NOSTATE;
    SHA256_Init(reinterpret_cast<SHA256_ALG_INFO*>(m_ctx));
    return CKS_OK;
}

int CNChecksumSha256::ChecksumUpdate(unsigned char const* data, int len)
{
    if (m_ctx == 0)
        return CKS_ERR_NOSTATE;
    SHA256_Update(reinterpret_cast<SHA256_ALG_INFO*>(m_ctx),
                  const_cast<unsigned char*>(data), (unsigned long)len);
    return CKS_OK;
}

int CNChecksumSha256::ChecksumFinish(unsigned char* out, int outSize)
{
    if (m_ctx == 0)
        return CKS_ERR_NOSTATE;
    if (outSize < GetChecksumSize())
        return CKS_ERR_BUFSIZE;
    SHA256_Final(reinterpret_cast<SHA256_ALG_INFO*>(m_ctx), out);
    return CKS_OK;
}

int CNChecksumSha256::MakeChecksumFromMemory(unsigned char const* data, int len,
                                             unsigned char* out, int outSize)
{
    return MakeShaChecksumFromMemory(m_ctx, Hooks256(), data, len, out,
                                     outSize, GetChecksumSize());
}

int CNChecksumSha256::MakeChecksumFromFile(char const* path, unsigned char* out,
                                           int outSize)
{
    return MakeShaChecksumFromFile(m_ctx, Hooks256(), path, out, outSize,
                                   GetChecksumSize());
}

// ---- CNChecksumSha384（ORIG 0x08098a9c..0x08098e76）----

CNChecksumSha384::CNChecksumSha384()
    : IChecksum(CHECKSUM_TYPE_SHA384), m_ctx(0)
{
    m_ctx = new unsigned char[0xd0];
}

CNChecksumSha384::CNChecksumSha384(CNChecksumSha384 const&)
    : IChecksum(CHECKSUM_TYPE_SHA384), m_ctx(0)
{
}

CNChecksumSha384::~CNChecksumSha384()
{
    if (m_ctx != 0) {
        delete[] m_ctx;
        m_ctx = 0;
    }
}

CNChecksumSha384& CNChecksumSha384::operator=(CNChecksumSha384 const&)
{
    return *this;
}

int CNChecksumSha384::Initialize(unsigned char const*, int)
{ return CKS_ERR_DATALEN; }

int CNChecksumSha384::GetChecksumSize() const
{ return 0x30; }

int CNChecksumSha384::ChecksumStart()
{
    if (m_ctx == 0)
        return CKS_ERR_NOSTATE;
    SHA384_Init(reinterpret_cast<SHA384_ALG_INFO*>(m_ctx));
    return CKS_OK;
}

int CNChecksumSha384::ChecksumUpdate(unsigned char const* data, int len)
{
    if (m_ctx == 0)
        return CKS_ERR_NOSTATE;
    SHA384_Update(reinterpret_cast<SHA384_ALG_INFO*>(m_ctx),
                  const_cast<unsigned char*>(data), (unsigned long)len);
    return CKS_OK;
}

int CNChecksumSha384::ChecksumFinish(unsigned char* out, int outSize)
{
    if (m_ctx == 0)
        return CKS_ERR_NOSTATE;
    if (outSize < GetChecksumSize())
        return CKS_ERR_BUFSIZE;
    SHA384_Final(reinterpret_cast<SHA384_ALG_INFO*>(m_ctx), out);
    return CKS_OK;
}

int CNChecksumSha384::MakeChecksumFromMemory(unsigned char const* data, int len,
                                             unsigned char* out, int outSize)
{
    return MakeShaChecksumFromMemory(m_ctx, Hooks384(), data, len, out,
                                     outSize, GetChecksumSize());
}

int CNChecksumSha384::MakeChecksumFromFile(char const* path, unsigned char* out,
                                           int outSize)
{
    return MakeShaChecksumFromFile(m_ctx, Hooks384(), path, out, outSize,
                                   GetChecksumSize());
}

// ---- CNChecksumSha512（ORIG 0x08098e80..0x0809925a）----

CNChecksumSha512::CNChecksumSha512()
    : IChecksum(CHECKSUM_TYPE_SHA512), m_ctx(0)
{
    m_ctx = new unsigned char[0xd0];
}

CNChecksumSha512::CNChecksumSha512(CNChecksumSha512 const&)
    : IChecksum(CHECKSUM_TYPE_SHA512), m_ctx(0)
{
}

CNChecksumSha512::~CNChecksumSha512()
{
    if (m_ctx != 0) {
        delete[] m_ctx;
        m_ctx = 0;
    }
}

CNChecksumSha512& CNChecksumSha512::operator=(CNChecksumSha512 const&)
{
    return *this;
}

int CNChecksumSha512::Initialize(unsigned char const*, int)
{ return CKS_ERR_DATALEN; }

int CNChecksumSha512::GetChecksumSize() const
{ return 0x40; }

int CNChecksumSha512::ChecksumStart()
{
    if (m_ctx == 0)
        return CKS_ERR_NOSTATE;
    SHA512_Init(reinterpret_cast<SHA512_ALG_INFO*>(m_ctx));
    return CKS_OK;
}

int CNChecksumSha512::ChecksumUpdate(unsigned char const* data, int len)
{
    if (m_ctx == 0)
        return CKS_ERR_NOSTATE;
    SHA512_Update(reinterpret_cast<SHA512_ALG_INFO*>(m_ctx),
                  const_cast<unsigned char*>(data), (unsigned long)len);
    return CKS_OK;
}

int CNChecksumSha512::ChecksumFinish(unsigned char* out, int outSize)
{
    if (m_ctx == 0)
        return CKS_ERR_NOSTATE;
    if (outSize < GetChecksumSize())
        return CKS_ERR_BUFSIZE;
    SHA512_Final(reinterpret_cast<SHA512_ALG_INFO*>(m_ctx), out);
    return CKS_OK;
}

int CNChecksumSha512::MakeChecksumFromMemory(unsigned char const* data, int len,
                                             unsigned char* out, int outSize)
{
    return MakeShaChecksumFromMemory(m_ctx, Hooks512(), data, len, out,
                                     outSize, GetChecksumSize());
}

int CNChecksumSha512::MakeChecksumFromFile(char const* path, unsigned char* out,
                                           int outSize)
{
    return MakeShaChecksumFromFile(m_ctx, Hooks512(), path, out, outSize,
                                   GetChecksumSize());
}

// ---------------------------------------------------------------------------
// SHA 内核（FIPS 180-4；Transform 数据大端读入，与 ORIG static 一致）
// ---------------------------------------------------------------------------

#define SHA_ROTR32(x, n) (((x) >> (n)) | ((x) << (32 - (n))))
#define SHA_ROTR64(x, n) (((x) >> (n)) | ((x) << (64 - (n))))

static const unsigned int k_sha256K[64] = {
    0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u,
    0x3956c25bu, 0x59f111f1u, 0x923f82a4u, 0xab1c5ed5u,
    0xd807aa98u, 0x12835b01u, 0x243185beu, 0x550c7dc3u,
    0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u, 0xc19bf174u,
    0xe49b69c1u, 0xefbe4786u, 0x0fc19dc6u, 0x240ca1ccu,
    0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau,
    0x983e5152u, 0xa831c66du, 0xb00327c8u, 0xbf597fc7u,
    0xc6e00bf3u, 0xd5a79147u, 0x06ca6351u, 0x14292967u,
    0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu, 0x53380d13u,
    0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u,
    0xa2bfe8a1u, 0xa81a664bu, 0xc24b8b70u, 0xc76c51a3u,
    0xd192e819u, 0xd6990624u, 0xf40e3585u, 0x106aa070u,
    0x19a4c116u, 0x1e376c08u, 0x2748774cu, 0x34b0bcb5u,
    0x391c0cb3u, 0x4ed8aa4au, 0x5b9cca4fu, 0x682e6ff3u,
    0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u,
    0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u
};

static const unsigned long long k_sha512K[80] = {
    0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL,
    0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL, 0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL,
    0xd807aa98a3030242ULL, 0x12835b0145706fbeULL, 0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
    0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL, 0xc19bf174cf692694ULL,
    0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL, 0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
    0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
    0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL, 0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL,
    0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL, 0x06ca6351e003826fULL, 0x142929670a0e6e70ULL,
    0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
    0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
    0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL, 0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL,
    0xd192e819d6ef5218ULL, 0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
    0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL, 0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL,
    0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL, 0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL,
    0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
    0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL,
    0xca273eceea26619cULL, 0xd186b8c721c0c207ULL, 0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL,
    0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL, 0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
    0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL, 0x431d67c49c100d4cULL,
    0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL, 0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
};

static unsigned int sha256BE(unsigned char const* p)
{
    return ((unsigned int)p[0] << 24) | ((unsigned int)p[1] << 16) |
           ((unsigned int)p[2] << 8) | (unsigned int)p[3];
}

static void sha256StoreBE(unsigned int v, unsigned char* p)
{
    p[0] = (unsigned char)(v >> 24);
    p[1] = (unsigned char)(v >> 16);
    p[2] = (unsigned char)(v >> 8);
    p[3] = (unsigned char)v;
}

static void sha512BE(unsigned char const* p, unsigned long long& v)
{
    v = 0;
    for (int i = 0; i < 8; ++i)
        v = (v << 8) | p[i];
}

static void sha512StoreBE(unsigned long long v, unsigned char* p)
{
    for (int i = 7; i >= 0; --i) {
        p[i] = (unsigned char)(v & 0xff);
        v >>= 8;
    }
}

static void SHA256_Transform(unsigned long* stateWords, unsigned long* dataWords)
{
    // ORIG static _ZL16SHA256_TransformPmS_：state/数据均为 unsigned long*，
// 元素宽随平台；算法规范为 32 位寄存器，统一经 u32 视图读写。
    unsigned int* state = reinterpret_cast<unsigned int*>(stateWords);
    unsigned int w[64];
    unsigned char const* raw = reinterpret_cast<unsigned char const*>(dataWords);
    for (int t = 0; t < 16; ++t)
        w[t] = sha256BE(raw + t * 4);
    for (int t = 16; t < 64; ++t) {
        unsigned int s0 = SHA_ROTR32(w[t-15], 7) ^ SHA_ROTR32(w[t-15], 18) ^ (w[t-15] >> 3);
        unsigned int s1 = SHA_ROTR32(w[t-2], 17) ^ SHA_ROTR32(w[t-2], 19) ^ (w[t-2] >> 10);
        w[t] = w[t-16] + s0 + w[t-7] + s1;
    }

    unsigned int a = state[0], b = state[1], c = state[2], d = state[3];
    unsigned int e = state[4], f = state[5], g = state[6], h = state[7];
    for (int t = 0; t < 64; ++t) {
        unsigned int S1 = SHA_ROTR32(e, 6) ^ SHA_ROTR32(e, 11) ^ SHA_ROTR32(e, 25);
        unsigned int ch = (e & f) ^ (~e & g);
        unsigned int t1 = h + S1 + ch + k_sha256K[t] + w[t];
        unsigned int S0 = SHA_ROTR32(a, 2) ^ SHA_ROTR32(a, 13) ^ SHA_ROTR32(a, 22);
        unsigned int mj = (a & b) ^ (a & c) ^ (b & c);
        unsigned int t2 = S0 + mj;
        h = g; g = f; f = e; e = d + t1;
        d = c; c = b; b = a; a = t1 + t2;
    }
    state[0] += a; state[1] += b; state[2] += c; state[3] += d;
    state[4] += e; state[5] += f; state[6] += g; state[7] += h;
}

static void sha512TransformCore(unsigned long long* state, unsigned char const* block)
{
    unsigned long long w[80];
    for (int t = 0; t < 16; ++t)
        sha512BE(block + t * 8, w[t]);
    for (int t = 16; t < 80; ++t) {
        unsigned long long s0 = SHA_ROTR64(w[t-15], 1) ^ SHA_ROTR64(w[t-15], 8) ^ (w[t-15] >> 7);
        unsigned long long s1 = SHA_ROTR64(w[t-2], 19) ^ SHA_ROTR64(w[t-2], 61) ^ (w[t-2] >> 6);
        w[t] = w[t-16] + s0 + w[t-7] + s1;
    }
    unsigned long long a=state[0], b=state[1], c=state[2], d=state[3];
    unsigned long long e=state[4], f=state[5], g=state[6], h=state[7];
    for (int t = 0; t < 80; ++t) {
        unsigned long long S1 = SHA_ROTR64(e,14) ^ SHA_ROTR64(e,18) ^ SHA_ROTR64(e,41);
        unsigned long long ch = (e & f) ^ (~e & g);
        unsigned long long t1 = h + S1 + ch + k_sha512K[t] + w[t];
        unsigned long long S0 = SHA_ROTR64(a,28) ^ SHA_ROTR64(a,34) ^ SHA_ROTR64(a,39);
        unsigned long long mj = (a & b) ^ (a & c) ^ (b & c);
        unsigned long long t2 = S0 + mj;
        h=g; g=f; f=e; e=d+t1; d=c; c=b; b=a; a=t1+t2;
    }
    state[0]+=a; state[1]+=b; state[2]+=c; state[3]+=d;
    state[4]+=e; state[5]+=f; state[6]+=g; state[7]+=h;
}

static void sha256Absorb(SHA256_ALG_INFO* ctx, unsigned char const* in,
                         unsigned long len)
{
    unsigned long long total =
        ((unsigned long long)ctx->total[1] << 32) | ctx->total[0];
    total += len;
    ctx->total[0] = (unsigned int)total;
    ctx->total[1] = (unsigned int)(total >> 32);

    unsigned long long done = total - len;   // 本次追加前的已处理量
    while (len > 0) {
        unsigned int pos = (unsigned int)(done % 64);
        unsigned long copy = 64 - pos;
        if (copy > len)
            copy = len;
        std::memcpy(ctx->buffer + pos, in, copy);
        done += copy;
        if (pos + copy == 64)
            SHA256_Transform(reinterpret_cast<unsigned long*>(&ctx->state[0]),
                             reinterpret_cast<unsigned long*>(ctx->buffer));
        in += copy;
        len -= copy;
    }
}

static void sha512Absorb(SHA512_ALG_INFO* ctx, unsigned char const* in,
                         unsigned long len)
{
    unsigned long long done = ctx->total[0];
    ctx->total[0] += (unsigned long long)len;
    while (len > 0) {
        unsigned int pos = (unsigned int)(done % 128u);
        unsigned long copy = 128 - pos;
        if (copy > len)
            copy = len;
        std::memcpy(reinterpret_cast<unsigned char*>(ctx->buffer) + pos, in, copy);
        done += (unsigned long long)copy;
        if (pos + copy == 128)
            sha512TransformCore(&ctx->state[0],
                                reinterpret_cast<unsigned char*>(ctx->buffer));
        in += copy;
        len -= copy;
    }
}

void SHA256_Init(SHA256_ALG_INFO* ctx)
{
    ctx->state[0] = 0x6a09e667u; ctx->state[1] = 0xbb67ae85u;
    ctx->state[2] = 0x3c6ef372u; ctx->state[3] = 0xa54ff53au;
    ctx->state[4] = 0x510e527fu; ctx->state[5] = 0x9b05688cu;
    ctx->state[6] = 0x1f83d9abu; ctx->state[7] = 0x5be0cd19u;
    ctx->total[1] = 0;
    ctx->total[0] = ctx->total[1];   // ORIG 0x080be075：先写 hi 再置 lo=hi(=0)
    std::memset(ctx->buffer, 0, sizeof(ctx->buffer));
}

void SHA256_Update(SHA256_ALG_INFO* ctx, unsigned char* in, unsigned long len)
{
    sha256Absorb(ctx, in, len);
}

void SHA256_Final(SHA256_ALG_INFO* ctx, unsigned char* out)
{   // ORIG 摘要口径：FIPS 180-4，长度域为比特数
    unsigned long long bytes =
        ((unsigned long long)ctx->total[1] << 32) | ctx->total[0];
    unsigned long long bits = bytes * 8uLL;
    unsigned char lenBytes[8];
    for (int i = 7; i >= 0; --i) {
        lenBytes[i] = (unsigned char)(bits & 0xff);
        bits >>= 8;
    }
    unsigned char tail[64];
    unsigned int pos = (unsigned int)(((unsigned long long)ctx->total[1] << 32 |
                                       ctx->total[0]) % 64);
    unsigned int padLen = (pos < 56) ? (56 - pos) : (120 - pos);
    tail[0] = 0x80;
    std::memset(tail + 1, 0, padLen - 1);
    std::memcpy(tail + padLen, lenBytes, 8);
    sha256Absorb(ctx, tail, padLen + 8);
    for (int i = 0; i < 8; ++i)
        sha256StoreBE(ctx->state[i], out + i * 4);
}

static void sha384Absorb(SHA384_ALG_INFO* ctx, unsigned char const* in,
                         unsigned long len)
{
    sha512Absorb(reinterpret_cast<SHA512_ALG_INFO*>(ctx), in, len);
}

void SHA384_Init(SHA384_ALG_INFO* ctx)
{
    ctx->state[0] = 0xcbbb9d5dc1059ed8ULL; ctx->state[1] = 0x629a292a367cd507ULL;
    ctx->state[2] = 0x9159015a3070dd17ULL; ctx->state[3] = 0x152fecd8f70e5939ULL;
    ctx->state[4] = 0x67332667ffc00b31ULL; ctx->state[5] = 0x8eb44a8768581511ULL;
    ctx->state[6] = 0xdb0c2e0d64f98fa7ULL; ctx->state[7] = 0x47b5481dbefa4fa4ULL;
    ctx->total[0] = 0;
    ctx->total[1] = 0;
    std::memset(ctx->buffer, 0, sizeof(ctx->buffer));
}

void SHA384_Update(SHA384_ALG_INFO* ctx, unsigned char* in, unsigned long len)
{
    sha384Absorb(ctx, in, len);
}

void SHA384_Final(SHA384_ALG_INFO* ctx, unsigned char* out)
{
    unsigned long long totalLo = ctx->total[0];
    unsigned char tail[128];
    unsigned char lenBytes[16];
    unsigned long long bits = totalLo * 8uLL;
    for (int i = 15; i >= 0; --i) {
        lenBytes[i] = (unsigned char)(bits & 0xff);
        bits >>= 8;
    }
    unsigned int pos = (unsigned int)(totalLo % 128u);
    unsigned int padLen = (pos < 112) ? (112 - pos) : (240 - pos);
    tail[0] = 0x80;
    std::memset(tail + 1, 0, padLen - 1);
    std::memcpy(tail + padLen, lenBytes, 16);
    sha384Absorb(ctx, tail, padLen + 16);
    for (int i = 0; i < 6; ++i)               // SHA-384 取前 48 字节
        sha512StoreBE(ctx->state[i], out + i * 8);
}

void SHA512_Init(SHA512_ALG_INFO* ctx)
{
    ctx->state[0] = 0x6a09e667f3bcc908ULL; ctx->state[1] = 0xbb67ae8584caa73bULL;
    ctx->state[2] = 0x3c6ef372fe94f82bULL; ctx->state[3] = 0xa54ff53a5f1d36f1ULL;
    ctx->state[4] = 0x510e527fade682d1ULL; ctx->state[5] = 0x9b05688c2b3e6c1fULL;
    ctx->state[6] = 0x1f83d9abfb41bd6bULL; ctx->state[7] = 0x5be0cd19137e2179ULL;
    ctx->total[0] = 0;
    ctx->total[1] = 0;
    std::memset(ctx->buffer, 0, sizeof(ctx->buffer));
}

void SHA512_Update(SHA512_ALG_INFO* ctx, unsigned char* in, unsigned long len)
{
    sha512Absorb(ctx, in, len);
}

void SHA512_Final(SHA512_ALG_INFO* ctx, unsigned char* out)
{
    unsigned long long totalLo = ctx->total[0];
    unsigned char tail[128];
    unsigned char lenBytes[16];
    unsigned long long bits = totalLo * 8uLL;
    for (int i = 15; i >= 0; --i) {
        lenBytes[i] = (unsigned char)(bits & 0xff);
        bits >>= 8;
    }
    unsigned int pos = (unsigned int)(totalLo % 128u);
    unsigned int padLen = (pos < 112) ? (112 - pos) : (240 - pos);
    tail[0] = 0x80;
    std::memset(tail + 1, 0, padLen - 1);
    std::memcpy(tail + padLen, lenBytes, 16);
    sha512Absorb(ctx, tail, padLen + 16);
    for (int i = 0; i < 8; ++i)
        sha512StoreBE(ctx->state[i], out + i * 8);
}

static void sha256HookInit(void* c)
{ SHA256_Init((SHA256_ALG_INFO*)c); }
static void sha256HookUpdate(void* c, unsigned char const* d, unsigned long n)
{ SHA256_Update((SHA256_ALG_INFO*)c, const_cast<unsigned char*>(d), n); }
static void sha256HookFinish(void* c, unsigned char* o)
{ SHA256_Final((SHA256_ALG_INFO*)c, o); }
static void sha384HookInit(void* c)
{ SHA384_Init((SHA384_ALG_INFO*)c); }
static void sha384HookUpdate(void* c, unsigned char const* d, unsigned long n)
{ SHA384_Update((SHA384_ALG_INFO*)c, const_cast<unsigned char*>(d), n); }
static void sha384HookFinish(void* c, unsigned char* o)
{ SHA384_Final((SHA384_ALG_INFO*)c, o); }
static void sha512HookInit(void* c)
{ SHA512_Init((SHA512_ALG_INFO*)c); }
static void sha512HookUpdate(void* c, unsigned char const* d, unsigned long n)
{ SHA512_Update((SHA512_ALG_INFO*)c, const_cast<unsigned char*>(d), n); }
static void sha512HookFinish(void* c, unsigned char* o)
{ SHA512_Final((SHA512_ALG_INFO*)c, o); }

// ---------------------------------------------------------------------------
// Rijndael 核心（Nb=4；SBox/InvSBox 为 FIPS-197 权威表）
// ---------------------------------------------------------------------------

static const unsigned char k_aesSBox[256] = {
    0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
    0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
    0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
    0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
    0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
    0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
    0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
    0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
    0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
    0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
    0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
    0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
    0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
    0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
    0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
    0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16
};

static const unsigned char k_aesInvSBox[256] = {
    0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38,0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb,
    0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87,0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb,
    0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d,0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e,
    0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2,0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25,
    0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16,0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92,
    0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda,0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84,
    0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a,0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06,
    0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02,0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b,
    0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea,0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73,
    0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85,0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e,
    0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89,0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b,
    0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20,0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4,
    0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31,0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f,
    0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d,0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef,
    0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0,0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61,
    0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26,0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d
};

static unsigned int aesXTime(unsigned int x)
{
    return ((x << 1) ^ ((x & 0x80) ? 0x1b : 0)) & 0xff;
}

static unsigned int aesMul(unsigned int x, unsigned int y)
{   // GF(2^8) 乘法
    unsigned int r = 0;
    while (y != 0) {
        if (y & 1)
            r ^= x;
        x = aesXTime(x);
        y >>= 1;
    }
    return r & 0xff;
}

// ORIG static _ZL20RIJNDAEL_KeySchedulePhmPm（0x0809ec59）：标准 Rijndael 密钥扩展。
// 输出 (Nr+1)*4 个轮密钥字（Nr = Nk + 6，Nb 固定 4，Nk = keyLen/4）。
static void RIJNDAEL_KeySchedule(unsigned char* key, unsigned long keyLen,
                                 unsigned long* rkOut)
{
    unsigned long nk = keyLen / 4;
    unsigned long totalWords = (nk + 6 + 1) * 4;

    std::memset(rkOut, 0, totalWords * sizeof(unsigned long));
    for (unsigned long i = 0; i < keyLen; ++i)
        rkOut[i >> 2] = (rkOut[i >> 2] << 8) | key[i];

    unsigned char rc = 0x01;
    for (unsigned long i = nk; i < totalWords; ++i) {
        unsigned long temp = rkOut[i - 1];
        if (i % nk == 0) {
            // RotWord + SubWord + Rcon（word 高位为 key 首字节）
            temp = ((temp << 8) | (temp >> 24));
            temp = ((unsigned long)k_aesSBox[(temp >> 24) & 0xff] << 24) |
                   ((unsigned long)k_aesSBox[(temp >> 16) & 0xff] << 16) |
                   ((unsigned long)k_aesSBox[(temp >> 8) & 0xff] << 8) |
                   (unsigned long)k_aesSBox[temp & 0xff];
            temp ^= ((unsigned long)rc << 24);
            if (rc < 0x80)
                rc = (unsigned char)(rc << 1);
            else
                rc = (unsigned char)((rc << 1) ^ 0x1b);
        } else if (nk > 6 && i % nk == 4) {
            temp = ((unsigned long)k_aesSBox[(temp >> 24) & 0xff] << 24) |
                   ((unsigned long)k_aesSBox[(temp >> 16) & 0xff] << 16) |
                   ((unsigned long)k_aesSBox[(temp >> 8) & 0xff] << 8) |
                   (unsigned long)k_aesSBox[temp & 0xff];
        }
        rkOut[i] = rkOut[i - nk] ^ temp;
    }
}

static void aesAddRoundKey(unsigned char st[16], unsigned long const* rk)
{
    for (int c = 0; c < 4; ++c) {
        unsigned long w = rk[c];
        st[c * 4 + 0] ^= (unsigned char)(w >> 24);
        st[c * 4 + 1] ^= (unsigned char)(w >> 16);
        st[c * 4 + 2] ^= (unsigned char)(w >> 8);
        st[c * 4 + 3] ^= (unsigned char)w;
    }
}

static void aesSubBytes(unsigned char st[16])
{
    for (int i = 0; i < 16; ++i)
        st[i] = k_aesSBox[st[i]];
}

static void aesShiftRows(unsigned char st[16])
{
    unsigned char t;
    t = st[1];  st[1] = st[5];   st[5] = st[9];   st[9] = st[13];  st[13] = t;
    t = st[2];  st[2] = st[10];  st[10] = t;
    t = st[6];  st[6] = st[14];  st[14] = t;
    t = st[15]; st[15] = st[11]; st[11] = st[7];  st[7] = st[3];   st[3] = t;
}

static void aesMixColumns(unsigned char st[16])
{
    for (int c = 0; c < 4; ++c) {
        unsigned char* p = st + c * 4;
        unsigned char a0 = p[0], a1 = p[1], a2 = p[2], a3 = p[3];
        p[0] = (unsigned char)(aesMul(a0, 2)  ^ aesMul(a1, 3)  ^ a2 ^ a3);
        p[1] = (unsigned char)(a0             ^ aesMul(a1, 2)  ^ aesMul(a2, 3) ^ a3);
        p[2] = (unsigned char)(a0             ^ a1             ^ aesMul(a2, 2)  ^ aesMul(a3, 3));
        p[3] = (unsigned char)(aesMul(a0, 3)  ^ a1             ^ a2             ^ aesMul(a3, 2));
    }
}

static void aesInvSubBytes(unsigned char st[16])
{
    for (int i = 0; i < 16; ++i)
        st[i] = k_aesInvSBox[st[i]];
}

static void aesInvShiftRows(unsigned char st[16])
{
    unsigned char t;
    t = st[13]; st[13] = st[9];  st[9] = st[5];  st[5] = st[1];   st[1] = t;
    t = st[2];  st[2] = st[10];  st[10] = t;
    t = st[6];  st[6] = st[14];  st[14] = t;
    t = st[3];  st[3] = st[7];   st[7] = st[11]; st[11] = st[15]; st[15] = t;
}

static void aesInvMixColumnsBlk(unsigned char st[16])
{
    for (int c = 0; c < 4; ++c) {
        unsigned char* p = st + c * 4;
        unsigned char a0 = p[0], a1 = p[1], a2 = p[2], a3 = p[3];
        p[0] = (unsigned char)(aesMul(a0, 14) ^ aesMul(a1, 11) ^ aesMul(a2, 13) ^ aesMul(a3, 9));
        p[1] = (unsigned char)(aesMul(a0, 9)  ^ aesMul(a1, 14) ^ aesMul(a2, 11) ^ aesMul(a3, 13));
        p[2] = (unsigned char)(aesMul(a0, 13) ^ aesMul(a1, 9)  ^ aesMul(a2, 14) ^ aesMul(a3, 11));
        p[3] = (unsigned char)(aesMul(a0, 11) ^ aesMul(a1, 13) ^ aesMul(a2, 9)  ^ aesMul(a3, 14));
    }
}

// ORIG AES_Encrypt(void*, unsigned char*) @0x0809f981：完全展开单块加密。
// 参数语义：pNumArea = &alg->num（结构体 +0x3c），ek 紧随其后（+0x40）。
void AES_Encrypt(void* pNumArea, unsigned char* block)
{
    unsigned long const* pNum = reinterpret_cast<unsigned long const*>(pNumArea);
    unsigned long num = pNum[0];
    unsigned long const* rk = pNum + 1;
    unsigned long nr = num + 6;      // Nb=4：Nk(4/6/8)+6 → 10/12/14 轮

    unsigned char state[16];
    std::memcpy(state, block, 16);
    aesAddRoundKey(state, rk);
    for (unsigned long r = 1; r < nr; ++r) {
        aesSubBytes(state);
        aesShiftRows(state);
        aesMixColumns(state);
        aesAddRoundKey(state, rk + r * 4);
    }
    aesSubBytes(state);
    aesShiftRows(state);
    aesAddRoundKey(state, rk + nr * 4);
    std::memcpy(block, state, 16);
}

void AES_Decrypt(void* pNumArea, unsigned char* block)
{
    unsigned long const* pNum = reinterpret_cast<unsigned long const*>(pNumArea);
    unsigned long num = pNum[0];
    unsigned long const* dk = pNum + 1;
    unsigned long nr = num + 6;

    unsigned char state[16];
    std::memcpy(state, block, 16);
    aesAddRoundKey(state, dk);
    for (unsigned long r = 1; r < nr; ++r) {
        aesInvSubBytes(state);
        aesInvShiftRows(state);
        aesAddRoundKey(state, dk + r * 4);       // dk 已做 InvMixColumn 预变换
        aesInvMixColumnsBlk(state);
    }
    aesInvSubBytes(state);
    aesInvShiftRows(state);
    aesAddRoundKey(state, dk + nr * 4);
    std::memcpy(block, state, 16);
}

// ---------------------------------------------------------------------------
// AES 模式骨架（public 符号；布局见 ChecksumCrypto.h）
// ---------------------------------------------------------------------------

void AES_SetAlgInfo(unsigned long mode, unsigned long padding,
                    unsigned char* vec, AES_ALG_INFO* alg)
{
    alg->mode = mode;
    alg->padding = padding;
    if (vec != 0)
        std::memcpy(alg->vec, vec, 16);
    else
        std::memset(alg->vec, 0, 16);
}

void AES_EncKeySchedule(unsigned char* key, unsigned long keyLen,
                        AES_ALG_INFO* alg)
{
    if (keyLen != 16 && keyLen != 24 && keyLen != 32)
        return;                      // ORIG 返错误码 0x1002 的 void 口径
    alg->num = (keyLen + 3) >> 2;
    RIJNDAEL_KeySchedule(key, keyLen, alg->rk);
}

void AES_DecKeySchedule(unsigned char* key, unsigned long keyLen,
                        AES_ALG_INFO* alg)
{
    if (keyLen != 16 && keyLen != 24 && keyLen != 32)
        return;
    alg->num = (keyLen + 3) >> 2;

    unsigned long esk[60];
    RIJNDAEL_KeySchedule(key, keyLen, esk);
    unsigned long nr = alg->num / 4 + 6;

    // ORIG 等效解密轮密钥：dk[0]=esk[nr]，中间组 InvMixColumn，dk[nr]=esk[0]
    std::memcpy(&alg->rk[0], &esk[nr * 4], 16);
    for (unsigned long r = 1; r < nr; ++r) {
        unsigned char col[16];
        for (int c = 0; c < 4; ++c) {
            unsigned long w = esk[(nr - r) * 4 + c];
            col[c * 4 + 0] = (unsigned char)(w >> 24);
            col[c * 4 + 1] = (unsigned char)(w >> 16);
            col[c * 4 + 2] = (unsigned char)(w >> 8);
            col[c * 4 + 3] = (unsigned char)w;
        }
        aesInvMixColumnsBlk(col);
        for (int c = 0; c < 4; ++c) {
            alg->rk[r * 4 + c] =
                ((unsigned long)col[c * 4 + 0] << 24) |
                ((unsigned long)col[c * 4 + 1] << 16) |
                ((unsigned long)col[c * 4 + 2] << 8) |
                (unsigned long)col[c * 4 + 3];
        }
    }
    std::memcpy(&alg->rk[nr * 4], &esk[0], 16);
}

void AES_EncInit(AES_ALG_INFO* alg)
{
    alg->buffered = 0;                    // ORIG 0x080a27ce
    if (alg->mode == 1)
        return;                           // ECB 无链初始化
    std::memcpy(alg->chain, alg->vec, 16); // ORIG 0x080a27fd：vec→chain
}

void AES_DecInit(AES_ALG_INFO* alg)
{
    alg->buffered = 0;
    if (alg->mode == 1)
        return;
    std::memcpy(alg->chain, alg->vec, 16);
}

// ORIG static _ZL7PaddSetPhmmm（0x080a26d8）
static unsigned long PaddSet(unsigned char* buf, unsigned long dataLen,
                             unsigned long blockLen, unsigned long padding)
{
    if (padding == 1) {
        if (dataLen == 0)
            return 0;
        return AES_ERR_PAD_OVERFLOW;     // 非块对齐即溢出错误
    }
    if (padding == 2) {
        unsigned long pad = blockLen - (dataLen % blockLen);
        std::memset(buf + dataLen, (int)(unsigned char)pad, pad);
        return dataLen + pad;
    }
    return AES_ERR_BAD_MODE;
}

// ORIG static _ZL9PaddCheckPhmm（0x080a273d）
static unsigned long PaddCheck(unsigned char const* buf, unsigned long len,
                               unsigned long padding)
{
    if (padding == 1)
        return 0;
    if (padding == 2) {
        if (len == 0)
            return AES_ERR_PAD_CHECK;
        unsigned char pad = buf[len - 1];
        if (pad == 0 || pad > len)
            return AES_ERR_PAD_CHECK;
        for (unsigned long i = 1; i <= pad; ++i) {
            if (buf[len - i] != pad)
                return AES_ERR_PAD_CHECK;
        }
        return pad;
    }
    return AES_ERR_BAD_MODE;
}

namespace {

// Update 公共骨架：残留凑块继续、尾块缓存；*outLen=已产出字节数。
// （ORIG ECB/CBC/OFB/CFB 各自 Update 反汇编同构）

int ecbEncUpdate(AES_ALG_INFO* alg, unsigned char* in, unsigned long inLen,
                 unsigned char* out, unsigned long* outLen)
{
    unsigned long produced = 0;
    if (alg->buffered > 0) {
        if (alg->buffered + inLen < 16) {
            std::memcpy(alg->buf + alg->buffered, in, inLen);
            alg->buffered += inLen;
            *outLen = 0;
            return 0;
        }
        unsigned long need = 16 - alg->buffered;
        std::memcpy(alg->buf + alg->buffered, in, need);
        in += need;
        inLen -= need;
        AES_Encrypt(&alg->num, alg->buf);
        std::memcpy(out, alg->buf, 16);
        out += 16;
        produced += 16;
        alg->buffered = 0;
    }
    while (inLen >= 16) {
        std::memcpy(out, in, 16);
        AES_Encrypt(&alg->num, out);
        out += 16;
        in += 16;
        inLen -= 16;
        produced += 16;
    }
    if (inLen > 0) {
        std::memcpy(alg->buf, in, inLen);
        alg->buffered = inLen;
    }
    *outLen = produced;
    return 0;
}

int cbcEncUpdate(AES_ALG_INFO* alg, unsigned char* in, unsigned long inLen,
                 unsigned char* out, unsigned long* outLen)
{   // ORIG CBC_EncUpdate 0x080a29dc：明文^chain 后加密，新链=密文
    unsigned long produced = 0;
    if (alg->buffered > 0) {
        if (alg->buffered + inLen < 16) {
            std::memcpy(alg->buf + alg->buffered, in, inLen);
            alg->buffered += inLen;
            *outLen = 0;
            return 0;
        }
        unsigned long need = 16 - alg->buffered;
        std::memcpy(alg->buf + alg->buffered, in, need);
        for (int i = 0; i < 16; ++i)
            alg->buf[i] ^= alg->chain[i];
        AES_Encrypt(&alg->num, alg->buf);
        std::memcpy(out, alg->buf, 16);
        std::memcpy(alg->chain, alg->buf, 16);
        out += 16;
        produced += 16;
        in += need;
        inLen -= need;
        alg->buffered = 0;
    }
    while (inLen >= 16) {
        for (int i = 0; i < 16; ++i)
            out[i] = (unsigned char)(in[i] ^ alg->chain[i]);
        AES_Encrypt(&alg->num, out);
        std::memcpy(alg->chain, out, 16);
        out += 16;
        in += 16;
        inLen -= 16;
        produced += 16;
    }
    if (inLen > 0) {
        std::memcpy(alg->buf, in, inLen);
        alg->buffered = inLen;
    }
    *outLen = produced;
    return 0;
}

int ecbDecUpdate(AES_ALG_INFO* alg, unsigned char* in, unsigned long inLen,
                 unsigned char* out, unsigned long* outLen)
{   // 保留最后一块整块供 DecFinal 校验 padding（ORIG *_DecUpdate 同构骨架）
    unsigned long avail = alg->buffered + inLen;
    if (avail < 32) {
        std::memcpy(alg->buf + alg->buffered, in, inLen);
        alg->buffered += inLen;
        *outLen = 0;
        return 0;
    }
    unsigned long produced = 0;
    if (alg->buffered > 0) {
        unsigned long need = 16 - alg->buffered;
        std::memcpy(alg->buf + alg->buffered, in, need);
        AES_Decrypt(&alg->num, alg->buf);
        std::memcpy(out, alg->buf, 16);
        out += 16;
        produced += 16;
        in += need;
        inLen -= need;
        alg->buffered = 0;
        avail -= 16;
    }
    // 剩余完整块中保留最后一块
    while (inLen >= 32) {
        std::memcpy(out, in, 16);
        AES_Decrypt(&alg->num, out);
        out += 16;
        in += 16;
        inLen -= 16;
        produced += 16;
    }
    if (inLen >= 16) {
        std::memcpy(alg->buf, in, inLen);   // 最后一块缓存至 buf
        alg->buffered = inLen;
        in += inLen;
        inLen = 0;
    } else if (inLen > 0) {
        std::memcpy(alg->buf + alg->buffered, in, inLen);
        alg->buffered += inLen;
    }
    *outLen = produced;
    return 0;
}

int cbcDecUpdate(AES_ALG_INFO* alg, unsigned char* in, unsigned long inLen,
                 unsigned char* out, unsigned long* outLen)
{
    unsigned long avail = alg->buffered + inLen;
    if (avail < 32) {
        std::memcpy(alg->buf + alg->buffered, in, inLen);
        alg->buffered += inLen;
        *outLen = 0;
        return 0;
    }
    unsigned long produced = 0;
    if (alg->buffered > 0) {
        unsigned long need = 16 - alg->buffered;
        std::memcpy(alg->buf + alg->buffered, in, need);
        for (int i = 0; i < 16; ++i) {
            unsigned char c = alg->buf[i];
            alg->buf[i] ^= alg->chain[i];
            alg->chain[i] = c;                  // 链更新为本密文块
        }
        AES_Decrypt(&alg->num, alg->buf);
        std::memcpy(out, alg->buf, 16);
        out += 16;
        produced += 16;
        in += need;
        inLen -= need;
        alg->buffered = 0;
    }
    while (inLen >= 32) {
        unsigned char cipher[16];
        std::memcpy(cipher, in, 16);
        for (int i = 0; i < 16; ++i)
            cipher[i] ^= alg->chain[i];
        AES_Decrypt(&alg->num, cipher);
        std::memcpy(out, cipher, 16);
        std::memcpy(alg->chain, in, 16);        // chain=本块密文
        out += 16;
        in += 16;
        inLen -= 16;
        produced += 16;
    }
    if (inLen >= 16) {
        std::memcpy(alg->buf, in, inLen);
        alg->buffered = inLen;
    }
    *outLen = produced;
    return 0;
}

// OFB：keystream = E(chain)，反馈 keystream 本身；流式无 padding。
int ofbEncUpdate(AES_ALG_INFO* alg, unsigned char* in, unsigned long inLen,
                 unsigned char* out, unsigned long* outLen)
{
    unsigned long produced = 0;
    if (alg->buffered > 0) {
        if (alg->buffered + inLen < 16) {
            std::memcpy(alg->buf + alg->buffered, in, inLen);
            alg->buffered += inLen;
            *outLen = 0;
            return 0;
        }
        unsigned long need = 16 - alg->buffered;
        std::memcpy(alg->buf + alg->buffered, in, need);
        AES_Encrypt(&alg->num, alg->chain);
        for (int i = 0; i < 16; ++i)
            alg->buf[i] ^= alg->chain[i];
        std::memcpy(out, alg->buf, 16);
        out += 16;
        produced += 16;
        in += need;
        inLen -= need;
        alg->buffered = 0;
    }
    while (inLen >= 16) {
        AES_Encrypt(&alg->num, alg->chain);
        for (int i = 0; i < 16; ++i)
            out[i] = (unsigned char)(in[i] ^ alg->chain[i]);
        out += 16;
        in += 16;
        inLen -= 16;
        produced += 16;
    }
    if (inLen > 0) {
        std::memcpy(alg->buf, in, inLen);
        alg->buffered = inLen;
    }
    *outLen = produced;
    return 0;
}

int ofbDecUpdate(AES_ALG_INFO* alg, unsigned char* in, unsigned long inLen,
                 unsigned char* out, unsigned long* outLen)
{   // OFB 加解密同构
    return ofbEncUpdate(alg, in, inLen, out, outLen);
}

// CFB：密文 = 明文 ^ E(chain)，密文回作新链。
int cfbEncUpdate(AES_ALG_INFO* alg, unsigned char* in, unsigned long inLen,
                 unsigned char* out, unsigned long* outLen)
{
    unsigned long produced = 0;
    if (alg->buffered > 0) {
        if (alg->buffered + inLen < 16) {
            std::memcpy(alg->buf + alg->buffered, in, inLen);
            alg->buffered += inLen;
            *outLen = 0;
            return 0;
        }
        unsigned long need = 16 - alg->buffered;
        std::memcpy(alg->buf + alg->buffered, in, need);
        AES_Encrypt(&alg->num, alg->chain);
        for (int i = 0; i < 16; ++i)
            alg->buf[i] ^= alg->chain[i];
        std::memcpy(out, alg->buf, 16);
        std::memcpy(alg->chain, alg->buf, 16);  // 新链=本段密文
        out += 16;
        produced += 16;
        in += need;
        inLen -= need;
        alg->buffered = 0;
    }
    while (inLen >= 16) {
        AES_Encrypt(&alg->num, alg->chain);
        for (int i = 0; i < 16; ++i)
            out[i] = (unsigned char)(in[i] ^ alg->chain[i]);
        std::memcpy(alg->chain, out, 16);
        out += 16;
        in += 16;
        inLen -= 16;
        produced += 16;
    }
    if (inLen > 0) {
        std::memcpy(alg->buf, in, inLen);
        alg->buffered = inLen;
    }
    *outLen = produced;
    return 0;
}

int cfbDecUpdate(AES_ALG_INFO* alg, unsigned char* in, unsigned long inLen,
                 unsigned char* out, unsigned long* outLen)
{   // 解密：明文 = 密文 ^ E(chain)；链更新为原密文。加解密形状一致但链取值不同，
    // 不能复用 enc 路径，独立转写。
    unsigned long produced = 0;
    if (alg->buffered > 0) {
        if (alg->buffered + inLen < 16) {
            std::memcpy(alg->buf + alg->buffered, in, inLen);
            alg->buffered += inLen;
            *outLen = 0;
            return 0;
        }
        unsigned long need = 16 - alg->buffered;
        std::memcpy(alg->buf + alg->buffered, in, need);
        AES_Encrypt(&alg->num, alg->chain);
        for (int i = 0; i < 16; ++i) {
            unsigned char plain = (unsigned char)(alg->buf[i] ^ alg->chain[i]);
            alg->chain[i] = alg->buf[i];        // 原(旧)密文段
            alg->buf[i] = plain;
        }
        std::memcpy(out, alg->buf, 16);
        out += 16;
        produced += 16;
        in += need;
        inLen -= need;
        alg->buffered = 0;
    }
    while (inLen >= 16) {
        unsigned char cipher[16];
        std::memcpy(cipher, in, 16);
        AES_Encrypt(&alg->num, alg->chain);
        for (int i = 0; i < 16; ++i)
            out[i] = (unsigned char)(cipher[i] ^ alg->chain[i]);
        std::memcpy(alg->chain, cipher, 16);
        out += 16;
        in += 16;
        inLen -= 16;
        produced += 16;
    }
    if (inLen > 0) {
        std::memcpy(alg->buf, in, inLen);
        alg->buffered = inLen;
    }
    *outLen = produced;
    return 0;
}

}  // namespace

// ---- ORIG static Enc Final（内部）----

namespace {

int ecbEncFinal(AES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen)
{   // ORIG ECB_EncFinal（结构对应 CBC 无填充版）：对残留补 0 后整块加密输出
    unsigned long len =
        PaddSet(alg->buf, alg->buffered, 16, alg->padding == 2 ? 2 : 1);
    if (len > 16)
        return (int)len;
    if (len == 0 && alg->padding != 2) {
        *outLen = 0;
        return 0;
    }
    AES_Encrypt(&alg->num, alg->buf);
    std::memcpy(out, alg->buf, 16);
    *outLen = 16;
    return 0;
}

}  // namespace

int AES_EncFinal(AES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen)
{
    switch (alg->mode) {
    case 1:
        return ecbEncFinal(alg, out, outLen);
    case 2: {   // ORIG CBC_EncFinal @0x080a336e
        unsigned long len = PaddSet(alg->buf, alg->buffered, 16, alg->padding);
        if (len > 16)
            return (int)len;
        if (len == 0) {
            *outLen = 0;
            return 0;
        }
        for (int i = 0; i < 16; ++i)
            alg->buf[i] ^= alg->chain[i];
        AES_Encrypt(&alg->num, alg->buf);
        std::memcpy(out, alg->buf, 16);
        std::memcpy(alg->chain, out, 16);
        *outLen = 16;
        return 0;
    }
    case 3: {   // ORIG OFB_EncFinal @0x080a34bd
        AES_Encrypt(&alg->num, alg->chain);
        for (unsigned long k = 0; k < alg->buffered; ++k)
            out[k] = (unsigned char)(alg->buf[k] ^ alg->chain[k]);
        *outLen = alg->buffered;
        return 0;
    }
    case 4: {   // CFB 同 OFB 形态
        AES_Encrypt(&alg->num, alg->chain);
        for (unsigned long k = 0; k < alg->buffered; ++k)
            out[k] = (unsigned char)(alg->buf[k] ^ alg->chain[k]);
        *outLen = alg->buffered;
        return 0;
    }
    default:
        return AES_ERR_BAD_MODE;
    }
}

// ---- ORIG public Dec Final 四种（T 符号 0x080a4396..0x080a47aa）----

int ECB_DecFinal(AES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen)
{
    AES_Decrypt(&alg->num, alg->buf);
    unsigned long pad = PaddCheck(alg->buf, 16, alg->padding);
    if (pad > 16 && pad != AES_ERR_BAD_MODE)
        return AES_ERR_PAD_CHECK;
    std::memcpy(out, alg->buf, 16 - pad);
    *outLen = 16 - pad;
    return 0;
}

int CBC_DecFinal(AES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen)
{
    unsigned char cipher[16];
    std::memcpy(cipher, alg->buf, 16);
    unsigned char plain[16];
    std::memcpy(plain, alg->buf, 16);
    AES_Decrypt(&alg->num, plain);
    for (int i = 0; i < 16; ++i)
        plain[i] ^= alg->chain[i];
    std::memcpy(alg->chain, cipher, 16);
    unsigned long pad = PaddCheck(plain, 16, alg->padding);
    if (pad > 16 && pad != AES_ERR_BAD_MODE)
        return AES_ERR_PAD_CHECK;
    std::memcpy(out, plain, 16 - pad);
    *outLen = 16 - pad;
    return 0;
}

int OFB_DecFinal(AES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen)
{
    AES_Encrypt(&alg->num, alg->chain);
    for (unsigned long k = 0; k < alg->buffered; ++k)
        out[k] = (unsigned char)(alg->buf[k] ^ alg->chain[k]);
    *outLen = alg->buffered;
    return 0;
}

int CFB_DecFinal(AES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen)
{
    AES_Encrypt(&alg->num, alg->chain);
    for (unsigned long k = 0; k < alg->buffered; ++k)
        out[k] = (unsigned char)(alg->buf[k] ^ alg->chain[k]);
    *outLen = alg->buffered;
    return 0;
}

int AES_EncUpdate(AES_ALG_INFO* alg, unsigned char* in, unsigned long inLen,
                  unsigned char* out, unsigned long* outLen)
{   // ORIG 0x080a31bc：按 alg->mode 分派到对应模式 Update
    switch (alg->mode) {
    case 1:
        return ecbEncUpdate(alg, in, inLen, out, outLen);
    case 2:
        return cbcEncUpdate(alg, in, inLen, out, outLen);
    case 3:
        return ofbEncUpdate(alg, in, inLen, out, outLen);
    case 4:
        return cfbEncUpdate(alg, in, inLen, out, outLen);
    default:
        return AES_ERR_BAD_MODE;               // ORIG 返 0x1001
    }
}

int AES_DecUpdate(AES_ALG_INFO* alg, unsigned char* in, unsigned long inLen,
                  unsigned char* out, unsigned long* outLen)
{
    switch (alg->mode) {
    case 1:
        return ecbDecUpdate(alg, in, inLen, out, outLen);
    case 2:
        return cbcDecUpdate(alg, in, inLen, out, outLen);
    case 3:
        return ofbDecUpdate(alg, in, inLen, out, outLen);
    case 4:
        return cfbDecUpdate(alg, in, inLen, out, outLen);
    default:
        return AES_ERR_BAD_MODE;
    }
}

int AES_DecFinal(AES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen)
{
    switch (alg->mode) {
    case 1:
        return ECB_DecFinal(alg, out, outLen);
    case 2:
        return CBC_DecFinal(alg, out, outLen);
    case 3:
        return OFB_DecFinal(alg, out, outLen);
    case 4:
        return CFB_DecFinal(alg, out, outLen);
    default:
        return AES_ERR_BAD_MODE;
    }
}

// ---------------------------------------------------------------------------
// CRijndael_fake（ORIG 0x080b824c..0x080b851c）
// 类布局：vptr@0x00、m_pCtxObf@0x04（存 new ptr ^ m_salt）、m_salt@0x08（ctor 随机）。
// 实际使用 Anubis-128/12轮 冒充 Rijndael（"fake"之名由此）——按 ORIG 忠实转写。
// ---------------------------------------------------------------------------

static unsigned char* fakeXor(void* p, unsigned int salt)
{
    return reinterpret_cast<unsigned char*>(reinterpret_cast<std::size_t>(p) ^ (std::size_t)salt);
}

CRijndael_fake::CRijndael_fake()
{   // ORIG 0x080b824c：salt=0xa321bcde ^ (rand()|rand()<<16)；new(0x268) 后 ptr^=salt
    m_salt = 0xa321bcdeu;
    unsigned int r1 = (unsigned int)std::rand();
    unsigned int r2 = (unsigned int)std::rand();
    m_salt ^= (r2 | (r1 << 16));
    m_ctxObf = new unsigned char[0x268];       // symmetric_key(anubis) 空间
    m_ctxObf = reinterpret_cast<void*>(reinterpret_cast<std::size_t>(m_ctxObf) ^ static_cast<std::size_t>(m_salt));
}

CRijndael_fake::~CRijndael_fake()
{   // ORIG 0x080b82c8：delete(ptr ^ salt)
    delete[] fakeXor(m_ctxObf, m_salt);
    m_ctxObf = 0;
}

int CRijndael_fake::Initialize(char const* key, int keyLen,
                               char const* /*chain*/, int /*chainLen*/,
                               int /*blockSize*/, int /*mode*/, int /*padding*/)
{
    if (keyLen <= 15)                          // ORIG cmpl $0xf
        return CKS_ERR_OPENFILE;               // 0x70000002
    symmetric_key* skey =
        reinterpret_cast<symmetric_key*>(fakeXor(m_ctxObf, m_salt));
    int rc = anubis_setup(reinterpret_cast<unsigned char const*>(key), 16, 12,
                          skey);
    if (rc == 0)
        return CKS_OK;                         // 0x6fffffff
    if (rc == CRYPT_INVALID_ROUNDS)
        return (int)0x7000001a;
    if (rc == CRYPT_INVALID_KEYSIZE)
        return CKS_ERR_OPENFILE;
    return CKS_ERR_NOSTATE;                    // ORIG 其它分支返 0x70000000
}

int CRijndael_fake::Encrypt(char const* in, char* out, unsigned int len)
{
    symmetric_key* skey =
        reinterpret_cast<symmetric_key*>(fakeXor(m_ctxObf, m_salt));
    unsigned int blocks = len >> 4;
    for (unsigned int i = 0; i < blocks; ++i) {
        anubis_ecb_encrypt(reinterpret_cast<unsigned char const*>(in),
                           reinterpret_cast<unsigned char*>(out), skey);
        in += 16;
        out += 16;
    }
    return CKS_OK;
}

int CRijndael_fake::Decrypt(char const* in, char* out, unsigned int len)
{
    symmetric_key* skey =
        reinterpret_cast<symmetric_key*>(fakeXor(m_ctxObf, m_salt));
    unsigned int blocks = len >> 4;
    for (unsigned int i = 0; i < blocks; ++i) {
        anubis_ecb_decrypt(reinterpret_cast<unsigned char const*>(in),
                           reinterpret_cast<unsigned char*>(out), skey);
        in += 16;
        out += 16;
    }
    return CKS_OK;
}

unsigned int CRijndael_fake::Pad(char* buffer, int pos, unsigned int* pLen)
{
    if (*pLen % GetBlockSize() != 0) {
        unsigned int pad = GetBlockSize() - (*pLen % GetBlockSize());
        std::memset(buffer + pos, 0, pad);      // ORIG：补零至块边界
        *pLen += pad;
    }
    return *pLen;
}

int CRijndael_fake::GetBlockSize() const
{
    return 0x10;
}

// ---------------------------------------------------------------------------
// 全局工厂（ORIG CreateCryptoChecksum 0x0809e309 / Destroy 0x0809e423）
// ---------------------------------------------------------------------------

IChecksum* CreateCryptoChecksum(CRYPTO_CHECKSUM_TYPE type)
{
    switch (type) {
    case CHECKSUM_TYPE_MD5:
        return new CNChecksumMd5();
    case CHECKSUM_TYPE_SHA256:
        return new CNChecksumSha256();
    case CHECKSUM_TYPE_SHA384:
        return new CNChecksumSha384();
    case CHECKSUM_TYPE_SHA512:
        return new CNChecksumSha512();
    default:
        return 0;
    }
}
