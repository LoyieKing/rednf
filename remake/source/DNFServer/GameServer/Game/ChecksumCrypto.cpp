// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ChecksumCrypto/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "ChecksumCrypto/IChecksum.inc"


#include "ChecksumCrypto/IChecksum_1.inc"


IChecksum& IChecksum::operator=(IChecksum const& other)
{
    return *this;   // ORIG 0x0809dee6：仅返回 this
}

#include "ChecksumCrypto/dtor_IChecksum.inc"


#include "ChecksumCrypto/DestroyCryptoChecksum.inc"


// ---------------------------------------------------------------------------
// CNChecksumMd5（ORIG 0x0809830c..0x080986ac）
// ---------------------------------------------------------------------------

#include "ChecksumCrypto/CNChecksumMd5.inc"


#include "ChecksumCrypto/dtor_CNChecksumMd5.inc"


#include "ChecksumCrypto/Initialize.inc"


#include "ChecksumCrypto/GetChecksumSize.inc"


#include "ChecksumCrypto/ChecksumStart.inc"


#include "ChecksumCrypto/ChecksumUpdate.inc"


#include "ChecksumCrypto/ChecksumFinish.inc"


#include "ChecksumCrypto/MakeChecksumFromMemory.inc"


#include "ChecksumCrypto/MakeChecksumFromFile.inc"


// ---------------------------------------------------------------------------
// CNChecksumSha256 / Sha384 / Sha512（显式实现；copy ctor/or= 与 ORIG 一致）
// ---------------------------------------------------------------------------

// ---- CNChecksumSha256（ORIG 0x080986b8..0x08098a9c）----

#include "ChecksumCrypto/CNChecksumSha256.inc"


#include "ChecksumCrypto/CNChecksumSha256_1.inc"


#include "ChecksumCrypto/dtor_CNChecksumSha256.inc"


CNChecksumSha256& CNChecksumSha256::operator=(CNChecksumSha256 const&)
{
    return *this;   // ORIG：空操作
}

#include "ChecksumCrypto/Initialize_1.inc"


#include "ChecksumCrypto/GetChecksumSize_1.inc"


#include "ChecksumCrypto/ChecksumStart_1.inc"


#include "ChecksumCrypto/ChecksumUpdate_1.inc"


#include "ChecksumCrypto/ChecksumFinish_1.inc"


#include "ChecksumCrypto/MakeChecksumFromMemory_1.inc"


#include "ChecksumCrypto/MakeChecksumFromFile_1.inc"


// ---- CNChecksumSha384（ORIG 0x08098a9c..0x08098e76）----

#include "ChecksumCrypto/CNChecksumSha384.inc"


#include "ChecksumCrypto/CNChecksumSha384_1.inc"


#include "ChecksumCrypto/dtor_CNChecksumSha384.inc"


CNChecksumSha384& CNChecksumSha384::operator=(CNChecksumSha384 const&)
{
    return *this;
}

#include "ChecksumCrypto/Initialize_2.inc"


#include "ChecksumCrypto/GetChecksumSize_2.inc"


#include "ChecksumCrypto/ChecksumStart_2.inc"


#include "ChecksumCrypto/ChecksumUpdate_2.inc"


#include "ChecksumCrypto/ChecksumFinish_2.inc"


#include "ChecksumCrypto/MakeChecksumFromMemory_2.inc"


#include "ChecksumCrypto/MakeChecksumFromFile_2.inc"


// ---- CNChecksumSha512（ORIG 0x08098e80..0x0809925a）----

#include "ChecksumCrypto/CNChecksumSha512.inc"


#include "ChecksumCrypto/CNChecksumSha512_1.inc"


#include "ChecksumCrypto/dtor_CNChecksumSha512.inc"


CNChecksumSha512& CNChecksumSha512::operator=(CNChecksumSha512 const&)
{
    return *this;
}

#include "ChecksumCrypto/Initialize_3.inc"


#include "ChecksumCrypto/GetChecksumSize_3.inc"


#include "ChecksumCrypto/ChecksumStart_3.inc"


#include "ChecksumCrypto/ChecksumUpdate_3.inc"


#include "ChecksumCrypto/ChecksumFinish_3.inc"


#include "ChecksumCrypto/MakeChecksumFromMemory_3.inc"


#include "ChecksumCrypto/MakeChecksumFromFile_3.inc"


// ---------------------------------------------------------------------------
// SHA 内核（FIPS 180-4；Transform 数据大端读入，与 ORIG static 一致）
// ---------------------------------------------------------------------------

#define SHA_ROTR32(x, n) (((x) >> (n)) | ((x) << (32 - (n))))
// [SemA14 2026-09-12 · ORIG 实证] ORIG 的 SHA512 内核用**有符号 64 位**语义：其 x86 序列
// 为 `shrd $n,%ebx,%ecx; sar $n,%ebx; shl $(64-n),%edx; or` —— `>>` 是**算术**右移
// （符号扩展），非标准 SHA512 的无符号逻辑右移。证据：mmap 回原址实跑 ORIG
// `_ZL16SHA512_TransformPmS_`(0x080c077c)，端到端 SHA512("abc")=e30e76550195626f…
// （≠ 标准 ddaf35a1…）；W[18]=σ1(W[16]) 只有把 >>19/>>61/>>6 全改算术右移才得
// ORIG 的 0xfe01827a17b3dffc。10 组向量（1B..4096B）修后逐字节相等。
#define SHA_ROTR64(x, n) ((unsigned long long)((((long long)(x)) >> (n)) | ((x) << (64 - (n)))))
#define SHA_SHR64(x, n) ((unsigned long long)(((long long)(x)) >> (n)))

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

// [b28d-Main·检查点18] 原 static 助手改宏（ORIG 无这些局部符号；宏展开零符号、行为等价）。
// [SemA14 2026-09-12 · ORIG 实证] ORIG 装载为 u32 直接读（无字节交换；大端机假设）。
#define sha256BE(p) (*(const unsigned long*)(p)) /* u32 direct load (ORIG) */

// [b28d-SymChecksum·检查点18同法] 原 static 助手改宏（ORIG 无该局部符号；宏展开零符号）。
// [SemA14 2026-09-12 · ORIG 实证] 长度域 = bswap32 后直写 = **大端**
// （Final 0x080be28d..0x080be2e3：ror/and/or 两处 + mov %eax,(%edx)）⇒ 保持逐字节 BE。
#define sha256StoreBE(v, p) do { \
    unsigned int _v = (unsigned int)(v); \
    unsigned char* _p = (unsigned char*)(p); \
    _p[0] = (unsigned char)(_v >> 24); \
    _p[1] = (unsigned char)(_v >> 16); \
    _p[2] = (unsigned char)(_v >> 8); \
    _p[3] = (unsigned char)_v; } while (0)

// [SemA14 2026-09-12 · ORIG 实证] 摘要输出 = **直写**（Final 0x080be303..0x080be319：
// `mov (%edx,%ecx,4),%edx; mov %edx,(%eax)`，无 bswap；长度域那份不适用）。
#define sha256StoreLE(v, p) do { *(unsigned long*)(p) = (unsigned long)(v); } while (0)

// [SemA14 2026-09-12 · ORIG 实证] ORIG 装载段（0x080c0791..0x080c07f3）为
// `mov (%eax),%eax; … or …; mov 0x4(%eax),%eax; … or …` = **两个 32 位半字交换**
// （低地址半字落**高** 32 位），全段无 bswap/逐字节拼装。实测 ORIG W[0]=
// 0x80636261_00000000（"abc\x80" 的 le32 在高位）、W[15]=0x18 ⇒ 与下式逐值吻合。
#define sha512BE(p, v) do { \
    const unsigned long* _p = (const unsigned long*)(p); \
    (v) = ((unsigned long long)_p[0] << 32) | (unsigned long long)_p[1]; } while (0)

// [b28d-SymChecksum·检查点18同法] 原 static 助手改宏（ORIG 无该局部符号；宏展开零符号）。
// [SemA14 2026-09-12 · ORIG 实证] 同上：ORIG 的存储亦为**两个 32 位半字交换**
// （高半字写**低**地址）。实测 ORIG SHA512_Final 写长度域后 ctx->buffer[112..127]
// = 00 00 00 00 | 18 00 00 00（值 0x18 的高半 0 在前），与下式一致。
#define sha512StoreBE(v, p) do { \
    unsigned long long _v = (v); \
    unsigned long* _p = (unsigned long*)(p); \
    _p[0] = (unsigned long)(_v >> 32); \
    _p[1] = (unsigned long)_v; } while (0)

static void SHA256_Transform(unsigned long* dataWords, unsigned long* stateWords)
{
    // ORIG static _ZL16SHA256_TransformPmS_（0x080bd978）：**参数序 (data, state)**
    // ——ORIG SHA256_Update 0x080be130 / SHA256_Final 0x080be23e 均压 (buffer, ctx)；
    // 旧实现写成 (state, data) 与 ORIG 契约相反（内部自洽但 ABI 语义颠倒，已改）。
    // 算法规范为 32 位寄存器，统一经 u32 视图读写。
    // [b28d-41] 豁免（用户规则 §6：SHA 通用算法类只保算法正确）：ORIG 4.1 将
    // 64 轮 W 扩展 + 64 轮压缩完全展开（584 条），重建用两段 for 循环（247 条）
    // ——语义等价。实测（/tmp/d41_crypto_test.cpp）：SHA256("abc") =
    // ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad ✓
    // （FIPS-180 向量，经 CNChecksumSha256::MakeChecksumFromMemory 全链）。
    // [b28d-solo-ChecksumCrypto] Transform 内核 N 封顶（算法族豁免，guide §6）：
    // ORIG 消息扩展单表达式 + 主循环 8 路展开共 584 条；重建 = 两段 for 循环 247 条。
    // 参数序已按 ORIG 修正为 (data, state)。
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

// ORIG static _ZL16SHA512_TransformPmS_（0x080c077c）/ _ZL16SHA384_TransformPmS_
// （0x080be32c）：参数序 (data, state)（ORIG SHA512_Update 0x080c29dd 压 (buffer, ctx)）。
// 两者为同体的独立函数（ORIG 侧各一份，0x2075 字节完全展开；此处循环表达，算法族豁免）。
#define SHA512_TRANSFORM_BODY() \
    unsigned long long* st = reinterpret_cast<unsigned long long*>(stateWords); \
    unsigned char const* bl = reinterpret_cast<unsigned char const*>(dataWords); \
    unsigned long long w[80]; \
    for (int t = 0; t < 16; ++t) sha512BE(bl + t * 8, w[t]); \
    for (int t = 16; t < 80; ++t) { \
        unsigned long long s0 = SHA_ROTR64(w[t-15], 1) ^ SHA_ROTR64(w[t-15], 8) ^ SHA_SHR64(w[t-15], 7); \
        unsigned long long s1 = SHA_ROTR64(w[t-2], 19) ^ SHA_ROTR64(w[t-2], 61) ^ SHA_SHR64(w[t-2], 6); \
        w[t] = w[t-16] + s0 + w[t-7] + s1; } \
    unsigned long long a=st[0], b=st[1], c=st[2], d=st[3]; \
    unsigned long long e=st[4], f=st[5], g=st[6], h=st[7]; \
    for (int t = 0; t < 80; ++t) { \
        unsigned long long S1 = SHA_ROTR64(e,14) ^ SHA_ROTR64(e,18) ^ SHA_ROTR64(e,41); \
        unsigned long long ch = (e & f) ^ (~e & g); \
        unsigned long long t1 = h + S1 + ch + k_sha512K[t] + w[t]; \
        unsigned long long S0 = SHA_ROTR64(a,28) ^ SHA_ROTR64(a,34) ^ SHA_ROTR64(a,39); \
        unsigned long long mj = (a & b) ^ (a & c) ^ (b & c); \
        unsigned long long t2 = S0 + mj; \
        h=g; g=f; f=e; e=d+t1; d=c; c=b; b=a; a=t1+t2; } \
    st[0]+=a; st[1]+=b; st[2]+=c; st[3]+=d; \
    st[4]+=e; st[5]+=f; st[6]+=g; st[7]+=h;

static void SHA512_Transform(unsigned long* dataWords, unsigned long* stateWords)
{
    SHA512_TRANSFORM_BODY()
}

static void SHA384_Transform(unsigned long* dataWords, unsigned long* stateWords)
{
    SHA512_TRANSFORM_BODY()
}

#include "ChecksumCrypto/SHA256_Init.inc"


#include "ChecksumCrypto/SHA256_Update.inc"


#include "ChecksumCrypto/SHA256_Final.inc"


#include "ChecksumCrypto/SHA384_Init.inc"


#include "ChecksumCrypto/SHA384_Update.inc"


#include "ChecksumCrypto/SHA384_Final.inc"


#include "ChecksumCrypto/SHA512_Init.inc"


#include "ChecksumCrypto/SHA512_Update.inc"


#include "ChecksumCrypto/SHA512_Final.inc"


// [b28d-Main·检查点18] 9 个 sha*Hook* 包装函数已删（见上方 Hooks* 直接指针赋值）。

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

#define aesXTime(x) ((unsigned int)((((unsigned int)(x) << 1) ^ \
    ((((unsigned int)(x)) & 0x80) ? 0x1b : 0)) & 0xff))

#define aesMul(x, y) ({ unsigned int _x = (unsigned int)(x), _y = (unsigned int)(y), _r = 0; \
    while (_y != 0) { if (_y & 1) _r ^= _x; _x = aesXTime(_x); _y >>= 1; } _r & 0xff; })

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

// [b28d-SymChecksum·检查点18同法] 下列 7 个 aes* 原 static 助手改宏
// （ORIG 无这些局部符号；实参先绑定局部、逐字保留求值顺序）。
#define aesAddRoundKey(st, rk) do { \
    unsigned char* _st = (unsigned char*)(st); \
    unsigned long const* _rk = (rk); \
    for (int _c = 0; _c < 4; ++_c) { \
        unsigned long _w = _rk[_c]; \
        _st[_c * 4 + 0] ^= (unsigned char)(_w >> 24); \
        _st[_c * 4 + 1] ^= (unsigned char)(_w >> 16); \
        _st[_c * 4 + 2] ^= (unsigned char)(_w >> 8); \
        _st[_c * 4 + 3] ^= (unsigned char)_w; } } while (0)

#define aesSubBytes(st) do { \
    unsigned char* _st = (unsigned char*)(st); \
    for (int _i = 0; _i < 16; ++_i) \
        _st[_i] = k_aesSBox[_st[_i]]; } while (0)

#define aesShiftRows(st) do { \
    unsigned char* _st = (unsigned char*)(st); \
    unsigned char _t; \
    _t = _st[1];  _st[1] = _st[5];   _st[5] = _st[9];   _st[9] = _st[13];  _st[13] = _t; \
    _t = _st[2];  _st[2] = _st[10];  _st[10] = _t; \
    _t = _st[6];  _st[6] = _st[14];  _st[14] = _t; \
    _t = _st[15]; _st[15] = _st[11]; _st[11] = _st[7];  _st[7] = _st[3];   _st[3] = _t; \
    } while (0)

#define aesMixColumns(st) do { \
    unsigned char* _st = (unsigned char*)(st); \
    for (int _c = 0; _c < 4; ++_c) { \
        unsigned char* _p = _st + _c * 4; \
        unsigned char _a0 = _p[0], _a1 = _p[1], _a2 = _p[2], _a3 = _p[3]; \
        _p[0] = (unsigned char)(aesMul(_a0, 2)  ^ aesMul(_a1, 3)  ^ _a2 ^ _a3); \
        _p[1] = (unsigned char)(_a0             ^ aesMul(_a1, 2)  ^ aesMul(_a2, 3) ^ _a3); \
        _p[2] = (unsigned char)(_a0             ^ _a1             ^ aesMul(_a2, 2)  ^ aesMul(_a3, 3)); \
        _p[3] = (unsigned char)(aesMul(_a0, 3)  ^ _a1             ^ _a2             ^ aesMul(_a3, 2)); \
    } } while (0)

#define aesInvSubBytes(st) do { \
    unsigned char* _st = (unsigned char*)(st); \
    for (int _i = 0; _i < 16; ++_i) \
        _st[_i] = k_aesInvSBox[_st[_i]]; } while (0)

#define aesInvShiftRows(st) do { \
    unsigned char* _st = (unsigned char*)(st); \
    unsigned char _t; \
    _t = _st[13]; _st[13] = _st[9];  _st[9] = _st[5];  _st[5] = _st[1];   _st[1] = _t; \
    _t = _st[2];  _st[2] = _st[10];  _st[10] = _t; \
    _t = _st[6];  _st[6] = _st[14];  _st[14] = _t; \
    _t = _st[3];  _st[3] = _st[7];   _st[7] = _st[11]; _st[11] = _st[15]; _st[15] = _t; \
    } while (0)

#define aesInvMixColumnsBlk(st) do { \
    unsigned char* _st = (unsigned char*)(st); \
    for (int _c = 0; _c < 4; ++_c) { \
        unsigned char* _p = _st + _c * 4; \
        unsigned char _a0 = _p[0], _a1 = _p[1], _a2 = _p[2], _a3 = _p[3]; \
        _p[0] = (unsigned char)(aesMul(_a0, 14) ^ aesMul(_a1, 11) ^ aesMul(_a2, 13) ^ aesMul(_a3, 9)); \
        _p[1] = (unsigned char)(aesMul(_a0, 9)  ^ aesMul(_a1, 14) ^ aesMul(_a2, 11) ^ aesMul(_a3, 13)); \
        _p[2] = (unsigned char)(aesMul(_a0, 13) ^ aesMul(_a1, 9)  ^ aesMul(_a2, 14) ^ aesMul(_a3, 11)); \
        _p[3] = (unsigned char)(aesMul(_a0, 11) ^ aesMul(_a1, 13) ^ aesMul(_a2, 9)  ^ aesMul(_a3, 14)); \
    } } while (0)

#include "ChecksumCrypto/AES_Encrypt.inc"


#include "ChecksumCrypto/AES_Decrypt.inc"


// ---------------------------------------------------------------------------
// AES 模式骨架（public 符号；布局见 ChecksumCrypto.h）
// ---------------------------------------------------------------------------

#include "ChecksumCrypto/AES_SetAlgInfo.inc"


#include "ChecksumCrypto/AES_EncKeySchedule.inc"


#include "ChecksumCrypto/AES_DecKeySchedule.inc"


#include "ChecksumCrypto/AES_EncInit.inc"


#include "ChecksumCrypto/AES_DecInit.inc"


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
static unsigned long PaddCheck(unsigned char* buf, unsigned long len,
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

// Update 公共骨架：残留凑块继续、尾块缓存；*outLen=已产出字节数。
// （ORIG ECB/CBC/OFB/CFB 各自 Update 反汇编同构）

static int ECB_EncUpdate(AES_ALG_INFO* alg, unsigned char* in, unsigned long inLen,
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

static int CBC_EncUpdate(AES_ALG_INFO* alg, unsigned char* in, unsigned long inLen,
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

static int ECB_DecUpdate(AES_ALG_INFO* alg, unsigned char* in, unsigned long inLen,
                 unsigned char* out, unsigned long* outLen)
{   // ORIG 0x080a3709（168 条，与 CBC 版同骨架但无链 XOR）：保留最后整块交 DecFinal。
    // [真 bug 修复] 旧实现 `while (inLen>=32)` + `memcpy(alg->buf,in,inLen)` 在
    // inLen∈[16,31] 时越界写 16 字节 buf；现按 ORIG 重写（残留 ≤ blockLen）。
    unsigned long* pNum = &alg->num;
    unsigned long blockLen = 16;
    unsigned long buffered = alg->buffered;
    *outLen = buffered + inLen;
    if (buffered + inLen <= blockLen) {
        std::memcpy(alg->buf + buffered, in, inLen);
        alg->buffered = alg->buffered + inLen;
        *outLen = 0;
        return 0;
    }
    if (in == out)
        return 0x1001;
    *outLen = buffered + inLen;
    std::memcpy(alg->buf + buffered, in, blockLen - buffered);
    in += blockLen - buffered;
    inLen += buffered - blockLen;
    ((unsigned long*)out)[0] = ((unsigned long*)alg->buf)[0];
    ((unsigned long*)out)[1] = ((unsigned long*)alg->buf)[1];
    ((unsigned long*)out)[2] = ((unsigned long*)alg->buf)[2];
    ((unsigned long*)out)[3] = ((unsigned long*)alg->buf)[3];
    AES_Decrypt(pNum, out);
    out += blockLen;
    while (inLen > blockLen) {
        ((unsigned long*)out)[0] = ((unsigned long*)in)[0];
        ((unsigned long*)out)[1] = ((unsigned long*)in)[1];
        ((unsigned long*)out)[2] = ((unsigned long*)in)[2];
        ((unsigned long*)out)[3] = ((unsigned long*)in)[3];
        AES_Decrypt(pNum, out);
        in += blockLen;
        out += blockLen;
        inLen -= blockLen;
    }
    std::memcpy(alg->buf, in, inLen);
    alg->buffered = (alg->buffered & 0xf0000000) + inLen;
    *outLen -= inLen;
    return 0;
}

static int CBC_DecUpdate(AES_ALG_INFO* alg, unsigned char* in, unsigned long inLen,
                 unsigned char* out, unsigned long* outLen)
{   // ORIG 0x080a38fa（与 DES 版同构，blockLen=16）：**先解密再 XOR 前一块密文**。
    // [真 bug 修复] 旧实现先 XOR chain 再 AES_Decrypt，AES-128-CBC NIST SP800-38A
    // F.2.1 向量实测解密错误；现按 ORIG 骨架重写（pNum 局部 + 保留最后整块）。
    unsigned long* pNum = &alg->num;
    unsigned long blockLen = 16;
    unsigned long buffered = alg->buffered;
    *outLen = buffered + inLen;
    if (buffered + inLen <= blockLen) {
        std::memcpy(alg->buf + buffered, in, inLen);
        alg->buffered = alg->buffered + inLen;
        *outLen = 0;
        return 0;
    }
    if (in == out)
        return 0x1001;
    *outLen = buffered + inLen;
    std::memcpy(alg->buf + buffered, in, blockLen - buffered);
    in += blockLen - buffered;
    inLen += buffered - blockLen;
    ((unsigned long*)out)[0] = ((unsigned long*)alg->buf)[0];
    ((unsigned long*)out)[1] = ((unsigned long*)alg->buf)[1];
    ((unsigned long*)out)[2] = ((unsigned long*)alg->buf)[2];
    ((unsigned long*)out)[3] = ((unsigned long*)alg->buf)[3];
    AES_Decrypt(pNum, out);
    ((unsigned long*)out)[0] = ((unsigned long*)out)[0] ^ ((unsigned long*)alg->chain)[0];
    ((unsigned long*)out)[1] = ((unsigned long*)out)[1] ^ ((unsigned long*)alg->chain)[1];
    ((unsigned long*)out)[2] = ((unsigned long*)out)[2] ^ ((unsigned long*)alg->chain)[2];
    ((unsigned long*)out)[3] = ((unsigned long*)out)[3] ^ ((unsigned long*)alg->chain)[3];
    out += blockLen;
    if (inLen <= blockLen) {
        ((unsigned long*)alg->chain)[0] = ((unsigned long*)alg->buf)[0];
        ((unsigned long*)alg->chain)[1] = ((unsigned long*)alg->buf)[1];
        ((unsigned long*)alg->chain)[2] = ((unsigned long*)alg->buf)[2];
        ((unsigned long*)alg->chain)[3] = ((unsigned long*)alg->buf)[3];
    } else {
        if (inLen > blockLen) {
            ((unsigned long*)out)[0] = ((unsigned long*)in)[0];
            ((unsigned long*)out)[1] = ((unsigned long*)in)[1];
            ((unsigned long*)out)[2] = ((unsigned long*)in)[2];
            ((unsigned long*)out)[3] = ((unsigned long*)in)[3];
            AES_Decrypt(pNum, out);
            ((unsigned long*)out)[0] = ((unsigned long*)out)[0] ^ ((unsigned long*)alg->buf)[0];
            ((unsigned long*)out)[1] = ((unsigned long*)out)[1] ^ ((unsigned long*)alg->buf)[1];
            ((unsigned long*)out)[2] = ((unsigned long*)out)[2] ^ ((unsigned long*)alg->buf)[2];
            ((unsigned long*)out)[3] = ((unsigned long*)out)[3] ^ ((unsigned long*)alg->buf)[3];
            in += blockLen;
            out += blockLen;
            inLen -= blockLen;
        }
        while (inLen > blockLen) {
            ((unsigned long*)out)[0] = ((unsigned long*)in)[0];
            ((unsigned long*)out)[1] = ((unsigned long*)in)[1];
            ((unsigned long*)out)[2] = ((unsigned long*)in)[2];
            ((unsigned long*)out)[3] = ((unsigned long*)in)[3];
            AES_Decrypt(pNum, out);
            ((unsigned long*)out)[0] = ((unsigned long*)out)[0] ^ ((unsigned long*)(in - blockLen))[0];
            ((unsigned long*)out)[1] = ((unsigned long*)out)[1] ^ ((unsigned long*)(in - blockLen))[1];
            ((unsigned long*)out)[2] = ((unsigned long*)out)[2] ^ ((unsigned long*)(in - blockLen))[2];
            ((unsigned long*)out)[3] = ((unsigned long*)out)[3] ^ ((unsigned long*)(in - blockLen))[3];
            in += blockLen;
            out += blockLen;
            inLen -= blockLen;
        }
        ((unsigned long*)alg->chain)[0] = ((unsigned long*)(in - blockLen))[0];
        ((unsigned long*)alg->chain)[1] = ((unsigned long*)(in - blockLen))[1];
        ((unsigned long*)alg->chain)[2] = ((unsigned long*)(in - blockLen))[2];
        ((unsigned long*)alg->chain)[3] = ((unsigned long*)(in - blockLen))[3];
    }
    std::memcpy(alg->buf, in, inLen);
    alg->buffered = (alg->buffered & 0xf0000000) + inLen;
    *outLen -= inLen;
    return 0;
}

static int OFB_EncUpdate(AES_ALG_INFO* alg, unsigned char* in, unsigned long inLen,
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

static int OFB_DecUpdate(AES_ALG_INFO* alg, unsigned char* in, unsigned long inLen,
                 unsigned char* out, unsigned long* outLen)
{   // ORIG 0x080a3d5e（208 条）：独立完整体（非转调 OFB_EncUpdate）；ORIG 直调
    // AES_Encrypt（p167 实证）。keystream = E(chain) 反馈自身，保留最后整块。
    unsigned long* pNum = &alg->num;
    unsigned long blockLen = 16;
    unsigned long buffered = alg->buffered;
    *outLen = buffered + inLen;
    if (buffered + inLen <= blockLen) {
        std::memcpy(alg->buf + buffered, in, inLen);
        alg->buffered = alg->buffered + inLen;
        *outLen = 0;
        return 0;
    }
    if (out == in)
        return 0x1001;
    *outLen = buffered + inLen;
    std::memcpy(alg->buf + buffered, in, blockLen - buffered);
    in += blockLen - buffered;
    inLen += buffered - blockLen;
    AES_Encrypt(pNum, alg->chain);
    ((unsigned long*)out)[0] = ((unsigned long*)alg->chain)[0] ^ ((unsigned long*)alg->buf)[0];
    ((unsigned long*)out)[1] = ((unsigned long*)alg->chain)[1] ^ ((unsigned long*)alg->buf)[1];
    ((unsigned long*)out)[2] = ((unsigned long*)alg->chain)[2] ^ ((unsigned long*)alg->buf)[2];
    ((unsigned long*)out)[3] = ((unsigned long*)alg->chain)[3] ^ ((unsigned long*)alg->buf)[3];
    out += blockLen;
    while (inLen > blockLen) {
        AES_Encrypt(pNum, alg->chain);
        ((unsigned long*)out)[0] = ((unsigned long*)alg->chain)[0] ^ ((unsigned long*)in)[0];
        ((unsigned long*)out)[1] = ((unsigned long*)alg->chain)[1] ^ ((unsigned long*)in)[1];
        ((unsigned long*)out)[2] = ((unsigned long*)alg->chain)[2] ^ ((unsigned long*)in)[2];
        ((unsigned long*)out)[3] = ((unsigned long*)alg->chain)[3] ^ ((unsigned long*)in)[3];
        in += blockLen;
        out += blockLen;
        inLen -= blockLen;
    }
    std::memcpy(alg->buf, in, inLen);
    alg->buffered = (alg->buffered & 0xf0000000) + inLen;
    *outLen -= inLen;
    return 0;
}

static int CFB_EncUpdate(AES_ALG_INFO* alg, unsigned char* in, unsigned long inLen,
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

static int CFB_DecUpdate(AES_ALG_INFO* alg, unsigned char* in, unsigned long inLen,
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

// ---- ORIG static Enc Final（内部，4 个独立 _ZL12*_EncFinalP12AES_ALG_INFOPhPm）----

static int ECB_EncFinal(AES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen)
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

static int CBC_EncFinal(AES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen)
{   // ORIG CBC_EncFinal @0x080a336e
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

static int OFB_EncFinal(AES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen)
{   // ORIG OFB_EncFinal @0x080a34bd
    AES_Encrypt(&alg->num, alg->chain);
    for (unsigned long k = 0; k < alg->buffered; ++k)
        out[k] = (unsigned char)(alg->buf[k] ^ alg->chain[k]);
    *outLen = alg->buffered;
    return 0;
}

static int CFB_EncFinal(AES_ALG_INFO* alg, unsigned char* out, unsigned long* outLen)
{   // CFB 同 OFB 形态
    AES_Encrypt(&alg->num, alg->chain);
    for (unsigned long k = 0; k < alg->buffered; ++k)
        out[k] = (unsigned char)(alg->buf[k] ^ alg->chain[k]);
    *outLen = alg->buffered;
    return 0;
}

#include "ChecksumCrypto/AES_EncFinal.inc"


// ---- ORIG public Dec Final 四种（T 符号 0x080a4396..0x080a47aa）----

#include "ChecksumCrypto/ECB_DecFinal.inc"


#include "ChecksumCrypto/CBC_DecFinal.inc"


#include "ChecksumCrypto/OFB_DecFinal.inc"


#include "ChecksumCrypto/CFB_DecFinal.inc"


#include "ChecksumCrypto/AES_EncUpdate.inc"


#include "ChecksumCrypto/AES_DecUpdate.inc"


// DES / 3DES（ORIG 0x080abcbc..0x080b14a0 / 0x080addf8..0x080afa08）
// 算法族语义正确真实现（FIPS 46-3 与 3DES-EDE）；布局/错误码/分派按 ORIG 反汇编。
// rk 区每轮 8 字节（6 字节 48 位子密钥 + 2 字节对齐填充），16 轮 = 0x80；
// DES3 为 3 × 0x80（K1 加密序 | K2 | K3，KeySchedule 直存，解密由轮序处理）。
// ---------------------------------------------------------------------------

// FIPS 46-3 置换/扩展表（1-based 位序）
static const unsigned char k_desIP[64] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
};

static const unsigned char k_desFP[64] = {
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9, 49, 17, 57, 25
};

static const unsigned char k_desE[48] = {
    32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9,
    8, 9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1
};

static const unsigned char k_desP[32] = {
    16, 7, 20, 21, 29, 12, 28, 17,
    1, 15, 23, 26, 5, 18, 31, 10,
    2, 8, 24, 14, 32, 27, 3, 9,
    19, 13, 30, 6, 22, 11, 4, 25
};

static const unsigned char k_desPC1[56] = {
    57, 49, 41, 33, 25, 17, 9,
    1, 58, 50, 42, 34, 26, 18,
    10, 2, 59, 51, 43, 35, 27,
    19, 11, 3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,
    7, 62, 54, 46, 38, 30, 22,
    14, 6, 61, 53, 45, 37, 29,
    21, 13, 5, 28, 20, 12, 4
};

static const unsigned char k_desPC2[48] = {
    14, 17, 11, 24, 1, 5,
    3, 28, 15, 6, 21, 10,
    23, 19, 12, 4, 26, 8,
    16, 7, 27, 20, 13, 2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32
};

static const unsigned char k_desShifts[16] = {
    1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

// FIPS 46-3 S 盒（行 0..3，每盒 64 项）
static const unsigned char k_desS[8][64] = {
    { 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
      0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
      4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
      15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13 },
    { 15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
      3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
      0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
      13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9 },
    { 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
      13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
      13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
      1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12 },
    { 7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
      13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
      10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
      3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14 },
    { 2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
      14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
      4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
      11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3 },
    { 12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
      10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
      9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
      4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13 },
    { 4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
      13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
      1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
      6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12 },
    { 13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
      1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
      7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
      2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11 }
};

// 弱/半弱密钥表（16 × 8 字节；ORIG DES_KeySchedule memcmp 表 @0x8b195a0）
static const unsigned char k_desWeakKeys[16][8] = {
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
    { 0x1E, 0x1E, 0x1E, 0x1E, 0x0F, 0x0F, 0x0F, 0x0F },
    { 0xE0, 0xE0, 0xE0, 0xE0, 0xF1, 0xF1, 0xF1, 0xF1 },
    { 0x01, 0xFE, 0x01, 0xFE, 0x01, 0xFE, 0x01, 0xFE },
    { 0xFE, 0x01, 0xFE, 0x01, 0xFE, 0x01, 0xFE, 0x01 },
    { 0x1F, 0xE0, 0x1F, 0xE0, 0x0E, 0xF1, 0x0E, 0xF1 },
    { 0xE0, 0x1F, 0xE0, 0x1F, 0xF1, 0x0E, 0xF1, 0x0E },
    { 0x01, 0xE0, 0x01, 0xE0, 0x01, 0xF1, 0x01, 0xF1 },
    { 0xE0, 0x01, 0xE0, 0x01, 0xF1, 0x01, 0xF1, 0x01 },
    { 0x1F, 0xFE, 0x1F, 0xFE, 0x0E, 0xFE, 0x0E, 0xFE },
    { 0xFE, 0x1F, 0xFE, 0x1F, 0xFE, 0x0E, 0xFE, 0x0E },
    { 0x01, 0x1F, 0x01, 0x1F, 0x01, 0x0E, 0x01, 0x0E },
    { 0x1F, 0x01, 0x1F, 0x01, 0x0E, 0x01, 0x0E, 0x01 },
    { 0xE0, 0xFE, 0xE0, 0xFE, 0xF1, 0xFE, 0xF1, 0xFE },
    { 0xFE, 0xE0, 0xFE, 0xE0, 0xFE, 0xF1, 0xFE, 0xF1 }
};

// 大端 64 位装载/存储（ORIG DES_Encrypt 对首 dword 做 ror24 字节交换 = 大端）
#define desLoad64(p) ({ const unsigned char* _p = (const unsigned char*)(p); \
    unsigned long long _v = 0; for (int _i = 0; _i < 8; ++_i) _v = (_v << 8) | _p[_i]; _v; })

// [b28d-SymChecksum·检查点18同法] 下列 3 个 des* 原 static 助手改宏
// （ORIG 无这些局部符号；实参先绑定局部、逐字保留求值顺序）。
#define desStore64(p, v) do { \
    unsigned char* _p = (unsigned char*)(p); \
    unsigned long long _v = (v); \
    for (int _i = 7; _i >= 0; --_i) { \
        _p[_i] = (unsigned char)(_v & 0xff); \
        _v >>= 8; } } while (0)

// 通用置换：tbl 为 1-based 位序，输入/输出均 MSB-first
#define desPermute(in, nbits, outbits, tbl) ({ \
    unsigned long long _in = (in); \
    int _nbits = (nbits), _outbits = (outbits); \
    unsigned char const* _tbl = (tbl); \
    unsigned long long _out = 0; \
    for (int _i = 0; _i < _outbits; ++_i) { \
        int _src = _tbl[_i] - 1; \
        _out = (_out << 1) | ((_in >> (_nbits - 1 - _src)) & 1ULL); } \
    _out; })

#define desRotl28(v, n) ((unsigned long)((((unsigned long)(v) << (n)) | \
    ((unsigned long)(v) >> (28 - (n)))) & 0x0fffffffUL))

// 第 round 轮 48 位子密钥（rk 区每轮 8 字节，前 6 字节有效）
#define desSubkey48(rk, round) ({ \
    unsigned char const* _rk = (rk); \
    int _round = (round); \
    unsigned long long _k = 0; \
    for (int _i = 0; _i < 6; ++_i) \
        _k = (_k << 8) | _rk[_round * 8 + _i]; \
    _k; })

// Feistel f(R, K)：E 扩展 → 异或 → S 盒 → P 置换
#define desF(r, subkey) ({ \
    unsigned long long _e = desPermute((r), 32, 48, k_desE) ^ (unsigned long long)(subkey); \
    unsigned long _f = 0; \
    for (int _i = 0; _i < 8; ++_i) { \
        int _b6 = (int)((_e >> (42 - _i * 6)) & 0x3f); \
        int _row = ((_b6 >> 4) & 2) | (_b6 & 1); \
        int _col = (_b6 >> 1) & 0xf; \
        _f = (_f << 4) | k_desS[_i][_row * 16 + _col]; } \
    (unsigned long)desPermute(_f, 32, 32, k_desP); })

// 16 轮核心以宏直接展开进导出函数体（O0 下不得是仅转调 helper 的短 wrapper，
// 否则 scan_stubs 按 ≤12 指令判桩；ENCRYPT=1 顺序 / 0 逆序）
#define DES_BLOCK_BODY(ENCRYPT)                                                \
    do {                                                                      \
        unsigned long long blk = desPermute(desLoad64(block), 64, 64, k_desIP);\
        unsigned long L = (unsigned long)(blk >> 32);                         \
        unsigned long R = (unsigned long)(blk & 0xffffffffUL);                \
        for (int round = 0; round < 16; ++round) {                            \
            int r = (ENCRYPT) ? round : (15 - round);                         \
            unsigned long t = L ^ desF(R, desSubkey48(                       \
                (unsigned char const*)rkArea, r));                            \
            L = R;                                                            \
            R = t;                                                            \
        }                                                                     \
        unsigned long long pre = ((unsigned long long)R << 32) | L;           \
        desStore64(block, desPermute(pre, 64, 64, k_desFP));                  \
    } while (0)

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// [SemA04 2026-09-12] DES 族（ORIG 变体）：单表承载（k_desTables）+ always_inline 助手。
// 依据：ORIG DES 族行为非标准（harness 实测 ORIG a8dbd7b9… ≠ 标准 3ccf04e2…），
// 故 DES_KeySchedule/DES_Encrypt/DES_Decrypt 逐指令转写自 ORIG 反汇编
// （0x080abcbc / 0x080ac0ba / 0x080acf59），表从 ORIG .rodata 原样提取。
// 布局：k_desTables[0..511]=des_skb、[512..1023]=des_SPtrans、[1024..1055]=弱密钥(字节视图)。
// 验证：ks 10/10、enc 10/10、dec 往返 10/10 与 ORIG mmap/Unicorn 逐字节一致。
// ---------------------------------------------------------------------------
#define DES_SHIFT_DEFINED
#define k_desSkb   (k_desTables)
#define k_desSPt   (k_desTables + 512)
#define k_desWeakKeysOrig ((const unsigned char*)(k_desTables + 1024))
static inline __attribute__((always_inline)) unsigned int shift_shr(unsigned int v, unsigned int n){ n&=31; return n?(v>>n):v; }
static inline __attribute__((always_inline)) unsigned int shift_shl(unsigned int v, unsigned int n){ n&=31; return n?(v<<n):v; }
static inline __attribute__((always_inline)) unsigned int shift_sar(unsigned int v, unsigned int n){ n&=31; return n?(unsigned int)((int)v>>n):v; }
static inline __attribute__((always_inline)) unsigned int shift_ror(unsigned int v, unsigned int n){ n&=31; return n?((v>>n)|(v<<(32-n))):v; }
static inline __attribute__((always_inline)) unsigned int shift_rol(unsigned int v, unsigned int n){ n&=31; return n?((v<<n)|(v>>(32-n))):v; }
static const unsigned int k_desTables[1056] = {
    0x00000000u, 0x00000010u, 0x20000000u, 0x20000010u, 0x00010000u, 0x00010010u, 0x20010000u, 0x20010010u,
    0x00000800u, 0x00000810u, 0x20000800u, 0x20000810u, 0x00010800u, 0x00010810u, 0x20010800u, 0x20010810u,
    0x00000020u, 0x00000030u, 0x20000020u, 0x20000030u, 0x00010020u, 0x00010030u, 0x20010020u, 0x20010030u,
    0x00000820u, 0x00000830u, 0x20000820u, 0x20000830u, 0x00010820u, 0x00010830u, 0x20010820u, 0x20010830u,
    0x00080000u, 0x00080010u, 0x20080000u, 0x20080010u, 0x00090000u, 0x00090010u, 0x20090000u, 0x20090010u,
    0x00080800u, 0x00080810u, 0x20080800u, 0x20080810u, 0x00090800u, 0x00090810u, 0x20090800u, 0x20090810u,
    0x00080020u, 0x00080030u, 0x20080020u, 0x20080030u, 0x00090020u, 0x00090030u, 0x20090020u, 0x20090030u,
    0x00080820u, 0x00080830u, 0x20080820u, 0x20080830u, 0x00090820u, 0x00090830u, 0x20090820u, 0x20090830u,
    0x00000000u, 0x02000000u, 0x00002000u, 0x02002000u, 0x00200000u, 0x02200000u, 0x00202000u, 0x02202000u,
    0x00000004u, 0x02000004u, 0x00002004u, 0x02002004u, 0x00200004u, 0x02200004u, 0x00202004u, 0x02202004u,
    0x00000400u, 0x02000400u, 0x00002400u, 0x02002400u, 0x00200400u, 0x02200400u, 0x00202400u, 0x02202400u,
    0x00000404u, 0x02000404u, 0x00002404u, 0x02002404u, 0x00200404u, 0x02200404u, 0x00202404u, 0x02202404u,
    0x10000000u, 0x12000000u, 0x10002000u, 0x12002000u, 0x10200000u, 0x12200000u, 0x10202000u, 0x12202000u,
    0x10000004u, 0x12000004u, 0x10002004u, 0x12002004u, 0x10200004u, 0x12200004u, 0x10202004u, 0x12202004u,
    0x10000400u, 0x12000400u, 0x10002400u, 0x12002400u, 0x10200400u, 0x12200400u, 0x10202400u, 0x12202400u,
    0x10000404u, 0x12000404u, 0x10002404u, 0x12002404u, 0x10200404u, 0x12200404u, 0x10202404u, 0x12202404u,
    0x00000000u, 0x00000001u, 0x00040000u, 0x00040001u, 0x01000000u, 0x01000001u, 0x01040000u, 0x01040001u,
    0x00000002u, 0x00000003u, 0x00040002u, 0x00040003u, 0x01000002u, 0x01000003u, 0x01040002u, 0x01040003u,
    0x00000200u, 0x00000201u, 0x00040200u, 0x00040201u, 0x01000200u, 0x01000201u, 0x01040200u, 0x01040201u,
    0x00000202u, 0x00000203u, 0x00040202u, 0x00040203u, 0x01000202u, 0x01000203u, 0x01040202u, 0x01040203u,
    0x08000000u, 0x08000001u, 0x08040000u, 0x08040001u, 0x09000000u, 0x09000001u, 0x09040000u, 0x09040001u,
    0x08000002u, 0x08000003u, 0x08040002u, 0x08040003u, 0x09000002u, 0x09000003u, 0x09040002u, 0x09040003u,
    0x08000200u, 0x08000201u, 0x08040200u, 0x08040201u, 0x09000200u, 0x09000201u, 0x09040200u, 0x09040201u,
    0x08000202u, 0x08000203u, 0x08040202u, 0x08040203u, 0x09000202u, 0x09000203u, 0x09040202u, 0x09040203u,
    0x00000000u, 0x00100000u, 0x00000100u, 0x00100100u, 0x00000008u, 0x00100008u, 0x00000108u, 0x00100108u,
    0x00001000u, 0x00101000u, 0x00001100u, 0x00101100u, 0x00001008u, 0x00101008u, 0x00001108u, 0x00101108u,
    0x04000000u, 0x04100000u, 0x04000100u, 0x04100100u, 0x04000008u, 0x04100008u, 0x04000108u, 0x04100108u,
    0x04001000u, 0x04101000u, 0x04001100u, 0x04101100u, 0x04001008u, 0x04101008u, 0x04001108u, 0x04101108u,
    0x00020000u, 0x00120000u, 0x00020100u, 0x00120100u, 0x00020008u, 0x00120008u, 0x00020108u, 0x00120108u,
    0x00021000u, 0x00121000u, 0x00021100u, 0x00121100u, 0x00021008u, 0x00121008u, 0x00021108u, 0x00121108u,
    0x04020000u, 0x04120000u, 0x04020100u, 0x04120100u, 0x04020008u, 0x04120008u, 0x04020108u, 0x04120108u,
    0x04021000u, 0x04121000u, 0x04021100u, 0x04121100u, 0x04021008u, 0x04121008u, 0x04021108u, 0x04121108u,
    0x00000000u, 0x10000000u, 0x00010000u, 0x10010000u, 0x00000004u, 0x10000004u, 0x00010004u, 0x10010004u,
    0x20000000u, 0x30000000u, 0x20010000u, 0x30010000u, 0x20000004u, 0x30000004u, 0x20010004u, 0x30010004u,
    0x00100000u, 0x10100000u, 0x00110000u, 0x10110000u, 0x00100004u, 0x10100004u, 0x00110004u, 0x10110004u,
    0x20100000u, 0x30100000u, 0x20110000u, 0x30110000u, 0x20100004u, 0x30100004u, 0x20110004u, 0x30110004u,
    0x00001000u, 0x10001000u, 0x00011000u, 0x10011000u, 0x00001004u, 0x10001004u, 0x00011004u, 0x10011004u,
    0x20001000u, 0x30001000u, 0x20011000u, 0x30011000u, 0x20001004u, 0x30001004u, 0x20011004u, 0x30011004u,
    0x00101000u, 0x10101000u, 0x00111000u, 0x10111000u, 0x00101004u, 0x10101004u, 0x00111004u, 0x10111004u,
    0x20101000u, 0x30101000u, 0x20111000u, 0x30111000u, 0x20101004u, 0x30101004u, 0x20111004u, 0x30111004u,
    0x00000000u, 0x08000000u, 0x00000008u, 0x08000008u, 0x00000400u, 0x08000400u, 0x00000408u, 0x08000408u,
    0x00020000u, 0x08020000u, 0x00020008u, 0x08020008u, 0x00020400u, 0x08020400u, 0x00020408u, 0x08020408u,
    0x00000001u, 0x08000001u, 0x00000009u, 0x08000009u, 0x00000401u, 0x08000401u, 0x00000409u, 0x08000409u,
    0x00020001u, 0x08020001u, 0x00020009u, 0x08020009u, 0x00020401u, 0x08020401u, 0x00020409u, 0x08020409u,
    0x02000000u, 0x0a000000u, 0x02000008u, 0x0a000008u, 0x02000400u, 0x0a000400u, 0x02000408u, 0x0a000408u,
    0x02020000u, 0x0a020000u, 0x02020008u, 0x0a020008u, 0x02020400u, 0x0a020400u, 0x02020408u, 0x0a020408u,
    0x02000001u, 0x0a000001u, 0x02000009u, 0x0a000009u, 0x02000401u, 0x0a000401u, 0x02000409u, 0x0a000409u,
    0x02020001u, 0x0a020001u, 0x02020009u, 0x0a020009u, 0x02020401u, 0x0a020401u, 0x02020409u, 0x0a020409u,
    0x00000000u, 0x00000100u, 0x00080000u, 0x00080100u, 0x01000000u, 0x01000100u, 0x01080000u, 0x01080100u,
    0x00000010u, 0x00000110u, 0x00080010u, 0x00080110u, 0x01000010u, 0x01000110u, 0x01080010u, 0x01080110u,
    0x00200000u, 0x00200100u, 0x00280000u, 0x00280100u, 0x01200000u, 0x01200100u, 0x01280000u, 0x01280100u,
    0x00200010u, 0x00200110u, 0x00280010u, 0x00280110u, 0x01200010u, 0x01200110u, 0x01280010u, 0x01280110u,
    0x00000200u, 0x00000300u, 0x00080200u, 0x00080300u, 0x01000200u, 0x01000300u, 0x01080200u, 0x01080300u,
    0x00000210u, 0x00000310u, 0x00080210u, 0x00080310u, 0x01000210u, 0x01000310u, 0x01080210u, 0x01080310u,
    0x00200200u, 0x00200300u, 0x00280200u, 0x00280300u, 0x01200200u, 0x01200300u, 0x01280200u, 0x01280300u,
    0x00200210u, 0x00200310u, 0x00280210u, 0x00280310u, 0x01200210u, 0x01200310u, 0x01280210u, 0x01280310u,
    0x00000000u, 0x04000000u, 0x00040000u, 0x04040000u, 0x00000002u, 0x04000002u, 0x00040002u, 0x04040002u,
    0x00002000u, 0x04002000u, 0x00042000u, 0x04042000u, 0x00002002u, 0x04002002u, 0x00042002u, 0x04042002u,
    0x00000020u, 0x04000020u, 0x00040020u, 0x04040020u, 0x00000022u, 0x04000022u, 0x00040022u, 0x04040022u,
    0x00002020u, 0x04002020u, 0x00042020u, 0x04042020u, 0x00002022u, 0x04002022u, 0x00042022u, 0x04042022u,
    0x00000800u, 0x04000800u, 0x00040800u, 0x04040800u, 0x00000802u, 0x04000802u, 0x00040802u, 0x04040802u,
    0x00002800u, 0x04002800u, 0x00042800u, 0x04042800u, 0x00002802u, 0x04002802u, 0x00042802u, 0x04042802u,
    0x00000820u, 0x04000820u, 0x00040820u, 0x04040820u, 0x00000822u, 0x04000822u, 0x00040822u, 0x04040822u,
    0x00002820u, 0x04002820u, 0x00042820u, 0x04042820u, 0x00002822u, 0x04002822u, 0x00042822u, 0x04042822u,
    0x02080800u, 0x00080000u, 0x02000002u, 0x02080802u, 0x02000000u, 0x00080802u, 0x00080002u, 0x02000002u,
    0x00080802u, 0x02080800u, 0x02080000u, 0x00000802u, 0x02000802u, 0x02000000u, 0x00000000u, 0x00080002u,
    0x00080000u, 0x00000002u, 0x02000800u, 0x00080800u, 0x02080802u, 0x02080000u, 0x00000802u, 0x02000800u,
    0x00000002u, 0x00000800u, 0x00080800u, 0x02080002u, 0x00000800u, 0x02000802u, 0x02080002u, 0x00000000u,
    0x00000000u, 0x02080802u, 0x02000800u, 0x00080002u, 0x02080800u, 0x00080000u, 0x00000802u, 0x02000800u,
    0x02080002u, 0x00000800u, 0x00080800u, 0x02000002u, 0x00080802u, 0x00000002u, 0x02000002u, 0x02080000u,
    0x02080802u, 0x00080800u, 0x02080000u, 0x02000802u, 0x02000000u, 0x00000802u, 0x00080002u, 0x00000000u,
    0x00080000u, 0x02000000u, 0x02000802u, 0x02080800u, 0x00000002u, 0x02080002u, 0x00000800u, 0x00080802u,
    0x40108010u, 0x00000000u, 0x00108000u, 0x40100000u, 0x40000010u, 0x00008010u, 0x40008000u, 0x00108000u,
    0x00008000u, 0x40100010u, 0x00000010u, 0x40008000u, 0x00100010u, 0x40108000u, 0x40100000u, 0x00000010u,
    0x00100000u, 0x40008010u, 0x40100010u, 0x00008000u, 0x00108010u, 0x40000000u, 0x00000000u, 0x00100010u,
    0x40008010u, 0x00108010u, 0x40108000u, 0x40000010u, 0x40000000u, 0x00100000u, 0x00008010u, 0x40108010u,
    0x00100010u, 0x40108000u, 0x40008000u, 0x00108010u, 0x40108010u, 0x00100010u, 0x40000010u, 0x00000000u,
    0x40000000u, 0x00008010u, 0x00100000u, 0x40100010u, 0x00008000u, 0x40000000u, 0x00108010u, 0x40008010u,
    0x40108000u, 0x00008000u, 0x00000000u, 0x40000010u, 0x00000010u, 0x40108010u, 0x00108000u, 0x40100000u,
    0x40100010u, 0x00100000u, 0x00008010u, 0x40008000u, 0x40008010u, 0x00000010u, 0x40100000u, 0x00108000u,
    0x04000001u, 0x04040100u, 0x00000100u, 0x04000101u, 0x00040001u, 0x04000000u, 0x04000101u, 0x00040100u,
    0x04000100u, 0x00040000u, 0x04040000u, 0x00000001u, 0x04040101u, 0x00000101u, 0x00000001u, 0x04040001u,
    0x00000000u, 0x00040001u, 0x04040100u, 0x00000100u, 0x00000101u, 0x04040101u, 0x00040000u, 0x04000001u,
    0x04040001u, 0x04000100u, 0x00040101u, 0x04040000u, 0x00040100u, 0x00000000u, 0x04000000u, 0x00040101u,
    0x04040100u, 0x00000100u, 0x00000001u, 0x00040000u, 0x00000101u, 0x00040001u, 0x04040000u, 0x04000101u,
    0x00000000u, 0x04040100u, 0x00040100u, 0x04040001u, 0x00040001u, 0x04000000u, 0x04040101u, 0x00000001u,
    0x00040101u, 0x04000001u, 0x04000000u, 0x04040101u, 0x00040000u, 0x04000100u, 0x04000101u, 0x00040100u,
    0x04000100u, 0x00000000u, 0x04040001u, 0x00000101u, 0x04000001u, 0x00040101u, 0x00000100u, 0x04040000u,
    0x00401008u, 0x10001000u, 0x00000008u, 0x10401008u, 0x00000000u, 0x10400000u, 0x10001008u, 0x00400008u,
    0x10401000u, 0x10000008u, 0x10000000u, 0x00001008u, 0x10000008u, 0x00401008u, 0x00400000u, 0x10000000u,
    0x10400008u, 0x00401000u, 0x00001000u, 0x00000008u, 0x00401000u, 0x10001008u, 0x10400000u, 0x00001000u,
    0x00001008u, 0x00000000u, 0x00400008u, 0x10401000u, 0x10001000u, 0x10400008u, 0x10401008u, 0x00400000u,
    0x10400008u, 0x00001008u, 0x00400000u, 0x10000008u, 0x00401000u, 0x10001000u, 0x00000008u, 0x10400000u,
    0x10001008u, 0x00000000u, 0x00001000u, 0x00400008u, 0x00000000u, 0x10400008u, 0x10401000u, 0x00001000u,
    0x10000000u, 0x10401008u, 0x00401008u, 0x00400000u, 0x10401008u, 0x00000008u, 0x10001000u, 0x00401008u,
    0x00400008u, 0x00401000u, 0x10400000u, 0x10001008u, 0x00001008u, 0x10000000u, 0x10000008u, 0x10401000u,
    0x08000000u, 0x00010000u, 0x00000400u, 0x08010420u, 0x08010020u, 0x08000400u, 0x00010420u, 0x08010000u,
    0x00010000u, 0x00000020u, 0x08000020u, 0x00010400u, 0x08000420u, 0x08010020u, 0x08010400u, 0x00000000u,
    0x00010400u, 0x08000000u, 0x00010020u, 0x00000420u, 0x08000400u, 0x00010420u, 0x00000000u, 0x08000020u,
    0x00000020u, 0x08000420u, 0x08010420u, 0x00010020u, 0x08010000u, 0x00000400u, 0x00000420u, 0x08010400u,
    0x08010400u, 0x08000420u, 0x00010020u, 0x08010000u, 0x00010000u, 0x00000020u, 0x08000020u, 0x08000400u,
    0x08000000u, 0x00010400u, 0x08010420u, 0x00000000u, 0x00010420u, 0x08000000u, 0x00000400u, 0x00010020u,
    0x08000420u, 0x00000400u, 0x00000000u, 0x08010420u, 0x08010020u, 0x08010400u, 0x00000420u, 0x00010000u,
    0x00010400u, 0x08010020u, 0x08000400u, 0x00000420u, 0x00000020u, 0x00010420u, 0x08010000u, 0x08000020u,
    0x80000040u, 0x00200040u, 0x00000000u, 0x80202000u, 0x00200040u, 0x00002000u, 0x80002040u, 0x00200000u,
    0x00002040u, 0x80202040u, 0x00202000u, 0x80000000u, 0x80002000u, 0x80000040u, 0x80200000u, 0x00202040u,
    0x00200000u, 0x80002040u, 0x80200040u, 0x00000000u, 0x00002000u, 0x00000040u, 0x80202000u, 0x80200040u,
    0x80202040u, 0x80200000u, 0x80000000u, 0x00002040u, 0x00000040u, 0x00202000u, 0x00202040u, 0x80002000u,
    0x00002040u, 0x80000000u, 0x80002000u, 0x00202040u, 0x80202000u, 0x00200040u, 0x00000000u, 0x80002000u,
    0x80000000u, 0x00002000u, 0x80200040u, 0x00200000u, 0x00200040u, 0x80202040u, 0x00202000u, 0x00000040u,
    0x80202040u, 0x00202000u, 0x00200000u, 0x80002040u, 0x80000040u, 0x80200000u, 0x00202040u, 0x00000000u,
    0x00002000u, 0x80000040u, 0x80002040u, 0x80202000u, 0x80200000u, 0x00002040u, 0x00000040u, 0x80200040u,
    0x00004000u, 0x00000200u, 0x01000200u, 0x01000004u, 0x01004204u, 0x00004004u, 0x00004200u, 0x00000000u,
    0x01000000u, 0x01000204u, 0x00000204u, 0x01004000u, 0x00000004u, 0x01004200u, 0x01004000u, 0x00000204u,
    0x01000204u, 0x00004000u, 0x00004004u, 0x01004204u, 0x00000000u, 0x01000200u, 0x01000004u, 0x00004200u,
    0x01004004u, 0x00004204u, 0x01004200u, 0x00000004u, 0x00004204u, 0x01004004u, 0x00000200u, 0x01000000u,
    0x00004204u, 0x01004000u, 0x01004004u, 0x00000204u, 0x00004000u, 0x00000200u, 0x01000000u, 0x01004004u,
    0x01000204u, 0x00004204u, 0x00004200u, 0x00000000u, 0x00000200u, 0x01000004u, 0x00000004u, 0x01000200u,
    0x00000000u, 0x01000204u, 0x01000200u, 0x00004200u, 0x00000204u, 0x00004000u, 0x01004204u, 0x01000000u,
    0x01004200u, 0x00000004u, 0x00004004u, 0x01004204u, 0x01000004u, 0x01004200u, 0x01004000u, 0x00004004u,
    0x20800080u, 0x20820000u, 0x00020080u, 0x00000000u, 0x20020000u, 0x00800080u, 0x20800000u, 0x20820080u,
    0x00000080u, 0x20000000u, 0x00820000u, 0x00020080u, 0x00820080u, 0x20020080u, 0x20000080u, 0x20800000u,
    0x00020000u, 0x00820080u, 0x00800080u, 0x20020000u, 0x20820080u, 0x20000080u, 0x00000000u, 0x00820000u,
    0x20000000u, 0x00800000u, 0x20020080u, 0x20800080u, 0x00800000u, 0x00020000u, 0x20820000u, 0x00000080u,
    0x00800000u, 0x00020000u, 0x20000080u, 0x20820080u, 0x00020080u, 0x20000000u, 0x00000000u, 0x00820000u,
    0x20800080u, 0x20020080u, 0x20020000u, 0x00800080u, 0x20820000u, 0x00000080u, 0x00800080u, 0x20020000u,
    0x20820080u, 0x00800000u, 0x20800000u, 0x20000080u, 0x00820000u, 0x00020080u, 0x20020080u, 0x20800000u,
    0x00000080u, 0x20820000u, 0x00820080u, 0x00000000u, 0x20000000u, 0x20800080u, 0x00020000u, 0x00820080u,
    0x01010101u, 0x01010101u, 0xfefefefeu, 0xfefefefeu, 0x1f1f1f1fu, 0x1f1f1f1fu, 0xe0e0e0e0u, 0xe0e0e0e0u,
    0xfe01fe01u, 0xfe01fe01u, 0x01fe01feu, 0x01fe01feu, 0xe01fe01fu, 0xf10ef10eu, 0x1fe01fe0u, 0x0ef10ef1u,
    0xe001e001u, 0xf101f101u, 0x01e001e0u, 0x01f101f1u, 0xfe1ffe1fu, 0xfe0efe0eu, 0x1ffe1ffeu, 0x0efe0efeu,
    0x1f011f01u, 0x0e010e01u, 0x011f011fu, 0x010e010eu, 0xfee0fee0u, 0xfef1fef1u, 0xe0fee0feu, 0xf1fef1feu,
};
#include "ChecksumCrypto/DES_KeySchedule.inc"


#include "ChecksumCrypto/DES3_KeySchedule.inc"


#include "ChecksumCrypto/DES3_Encrypt.inc"


#include "ChecksumCrypto/DES3_Decrypt.inc"


#include "ChecksumCrypto/DES_Encrypt.inc"


#include "ChecksumCrypto/DES_Decrypt.inc"


#include "ChecksumCrypto/DES_SetAlgInfo.inc"


#include "ChecksumCrypto/DES3_SetAlgInfo.inc"


#include "ChecksumCrypto/DES_EncInit.inc"


#include "ChecksumCrypto/DES_DecInit.inc"


#include "ChecksumCrypto/DES3_EncInit.inc"


#include "ChecksumCrypto/DES3_DecInit.inc"


// DES/DES3 模式骨架（file-static，ORIG _ZL13ECB_EncUpdateP12DES_ALG_INFO... 等；
// 与 AES 族 Update/Final 骨架同构，块大小 8）
// ---------------------------------------------------------------------------

static int ECB_EncUpdate(DES_ALG_INFO* alg, unsigned char* in,
                         unsigned long inLen, unsigned char* out,
                         unsigned long* outLen)
{
    unsigned long produced = 0;
    if (alg->buffered > 0) {
        if (alg->buffered + inLen < 8) {
            std::memcpy(alg->buf + alg->buffered, in, inLen);
            alg->buffered += inLen;
            *outLen = 0;
            return 0;
        }
        unsigned long need = 8 - alg->buffered;
        std::memcpy(alg->buf + alg->buffered, in, need);
        in += need;
        inLen -= need;
        DES_Encrypt(alg->rk, alg->buf);
        std::memcpy(out, alg->buf, 8);
        out += 8;
        produced += 8;
        alg->buffered = 0;
    }
    while (inLen >= 8) {
        std::memcpy(out, in, 8);
        DES_Encrypt(alg->rk, out);
        out += 8;
        in += 8;
        inLen -= 8;
        produced += 8;
    }
    if (inLen > 0) {
        std::memcpy(alg->buf, in, inLen);
        alg->buffered = inLen;
    }
    *outLen = produced;
    return 0;
}

static int CBC_EncUpdate(DES_ALG_INFO* alg, unsigned char* in,
                         unsigned long inLen, unsigned char* out,
                         unsigned long* outLen)
{   // ORIG CBC_EncUpdate 0x080afdbe：明文^chain 后加密，新链=密文
    unsigned long produced = 0;
    if (alg->buffered > 0) {
        if (alg->buffered + inLen < 8) {
            std::memcpy(alg->buf + alg->buffered, in, inLen);
            alg->buffered += inLen;
            *outLen = 0;
            return 0;
        }
        unsigned long need = 8 - alg->buffered;
        std::memcpy(alg->buf + alg->buffered, in, need);
        for (int i = 0; i < 8; ++i)
            alg->buf[i] = (unsigned char)(alg->buf[i] ^ alg->chain[i]);
        DES_Encrypt(alg->rk, alg->buf);
        std::memcpy(out, alg->buf, 8);
        std::memcpy(alg->chain, alg->buf, 8);
        out += 8;
        produced += 8;
        in += need;
        inLen -= need;
        alg->buffered = 0;
    }
    while (inLen >= 8) {
        for (int i = 0; i < 8; ++i)
            out[i] = (unsigned char)(in[i] ^ alg->chain[i]);
        DES_Encrypt(alg->rk, out);
        std::memcpy(alg->chain, out, 8);
        out += 8;
        in += 8;
        inLen -= 8;
        produced += 8;
    }
    if (inLen > 0) {
        std::memcpy(alg->buf, in, inLen);
        alg->buffered = inLen;
    }
    *outLen = produced;
    return 0;
}

static int OFB_EncUpdate(DES_ALG_INFO* alg, unsigned char* in,
                         unsigned long inLen, unsigned char* out,
                         unsigned long* outLen)
{   // keystream = E(chain)，反馈 keystream 本身；流式无 padding
    unsigned long produced = 0;
    if (alg->buffered > 0) {
        if (alg->buffered + inLen < 8) {
            std::memcpy(alg->buf + alg->buffered, in, inLen);
            alg->buffered += inLen;
            *outLen = 0;
            return 0;
        }
        unsigned long need = 8 - alg->buffered;
        std::memcpy(alg->buf + alg->buffered, in, need);
        DES_Encrypt(alg->rk, alg->chain);
        for (int i = 0; i < 8; ++i)
            alg->buf[i] = (unsigned char)(alg->buf[i] ^ alg->chain[i]);
        std::memcpy(out, alg->buf, 8);
        out += 8;
        produced += 8;
        in += need;
        inLen -= need;
        alg->buffered = 0;
    }
    while (inLen >= 8) {
        DES_Encrypt(alg->rk, alg->chain);
        for (int i = 0; i < 8; ++i)
            out[i] = (unsigned char)(in[i] ^ alg->chain[i]);
        out += 8;
        in += 8;
        inLen -= 8;
        produced += 8;
    }
    if (inLen > 0) {
        std::memcpy(alg->buf, in, inLen);
        alg->buffered = inLen;
    }
    *outLen = produced;
    return 0;
}

static int CFB_EncUpdate(DES_ALG_INFO* alg, unsigned char* in,
                         unsigned long inLen, unsigned char* out,
                         unsigned long* outLen)
{   // 密文 = 明文 ^ E(chain)，密文回作新链
    unsigned long produced = 0;
    if (alg->buffered > 0) {
        if (alg->buffered + inLen < 8) {
            std::memcpy(alg->buf + alg->buffered, in, inLen);
            alg->buffered += inLen;
            *outLen = 0;
            return 0;
        }
        unsigned long need = 8 - alg->buffered;
        std::memcpy(alg->buf + alg->buffered, in, need);
        DES_Encrypt(alg->rk, alg->chain);
        for (int i = 0; i < 8; ++i)
            alg->buf[i] = (unsigned char)(alg->buf[i] ^ alg->chain[i]);
        std::memcpy(out, alg->buf, 8);
        std::memcpy(alg->chain, alg->buf, 8);
        out += 8;
        produced += 8;
        in += need;
        inLen -= need;
        alg->buffered = 0;
    }
    while (inLen >= 8) {
        DES_Encrypt(alg->rk, alg->chain);
        for (int i = 0; i < 8; ++i)
            out[i] = (unsigned char)(in[i] ^ alg->chain[i]);
        std::memcpy(alg->chain, out, 8);
        out += 8;
        in += 8;
        inLen -= 8;
        produced += 8;
    }
    if (inLen > 0) {
        std::memcpy(alg->buf, in, inLen);
        alg->buffered = inLen;
    }
    *outLen = produced;
    return 0;
}

// ---- 解密 Update：保留最后一块整块供 DecFinal 校验 padding ----

static int ECB_DecUpdate(DES_ALG_INFO* alg, unsigned char* in,
                         unsigned long inLen, unsigned char* out,
                         unsigned long* outLen)
{
    unsigned long avail = alg->buffered + inLen;
    if (avail < 16) {
        std::memcpy(alg->buf + alg->buffered, in, inLen);
        alg->buffered += inLen;
        *outLen = 0;
        return 0;
    }
    unsigned long produced = 0;
    if (alg->buffered > 0) {
        unsigned long need = 8 - alg->buffered;
        std::memcpy(alg->buf + alg->buffered, in, need);
        DES_Decrypt(alg->rk, alg->buf);
        std::memcpy(out, alg->buf, 8);
        out += 8;
        produced += 8;
        in += need;
        inLen -= need;
        alg->buffered = 0;
    }
    while (inLen >= 16) {
        std::memcpy(out, in, 8);
        DES_Decrypt(alg->rk, out);
        out += 8;
        in += 8;
        inLen -= 8;
        produced += 8;
    }
    if (inLen >= 8) {
        std::memcpy(alg->buf, in, inLen);
        alg->buffered = inLen;
    } else if (inLen > 0) {
        std::memcpy(alg->buf + alg->buffered, in, inLen);
        alg->buffered += inLen;
    }
    *outLen = produced;
    return 0;
}

static int CBC_DecUpdate(DES_ALG_INFO* alg, unsigned char* in,
                         unsigned long inLen, unsigned char* out,
                         unsigned long* outLen)
{   // ORIG 0x080b09b6（CBC 解密独立完整体）：解密后 XOR 前一块密文；
    // 循环条件 `inLen > blockLen` ⇒ 残留 inLen∈[0,blockLen]，尾部 memcpy 不越界
    //（旧实现 `inLen >= 16` + `memcpy(buf,in,inLen)` 在 inLen%8!=0 时越界写 8 字节缓冲）。
    unsigned char* rk = alg->rk;
    unsigned long blockLen = 8;
    unsigned long buffered = alg->buffered;
    *outLen = buffered + inLen;
    if (buffered + inLen <= blockLen) {
        std::memcpy(alg->buf + buffered, in, inLen);
        alg->buffered = alg->buffered + inLen;
        *outLen = 0;
        return 0;
    }
    if (in == out)
        return 0x1001;
    *outLen = buffered + inLen;
    std::memcpy(alg->buf + buffered, in, blockLen - buffered);
    in += blockLen - buffered;
    inLen += buffered - blockLen;
    ((unsigned long*)out)[0] = ((unsigned long*)alg->buf)[0];
    ((unsigned long*)out)[1] = ((unsigned long*)alg->buf)[1];
    DES_Decrypt(rk, out);
    ((unsigned long*)out)[0] = ((unsigned long*)out)[0] ^ ((unsigned long*)alg->chain)[0];
    ((unsigned long*)out)[1] = ((unsigned long*)out)[1] ^ ((unsigned long*)alg->chain)[1];
    out += blockLen;
    if (inLen <= blockLen) {
        ((unsigned long*)alg->chain)[0] = ((unsigned long*)alg->buf)[0];
        ((unsigned long*)alg->chain)[1] = ((unsigned long*)alg->buf)[1];
    } else {
        if (inLen > blockLen) {
            ((unsigned long*)out)[0] = ((unsigned long*)in)[0];
            ((unsigned long*)out)[1] = ((unsigned long*)in)[1];
            DES_Decrypt(rk, out);
            ((unsigned long*)out)[0] = ((unsigned long*)out)[0] ^ ((unsigned long*)alg->buf)[0];
            ((unsigned long*)out)[1] = ((unsigned long*)out)[1] ^ ((unsigned long*)alg->buf)[1];
            in += blockLen;
            out += blockLen;
            inLen -= blockLen;
        }
        while (inLen > blockLen) {
            ((unsigned long*)out)[0] = ((unsigned long*)in)[0];
            ((unsigned long*)out)[1] = ((unsigned long*)in)[1];
            DES_Decrypt(rk, out);
            ((unsigned long*)out)[0] = ((unsigned long*)out)[0] ^ ((unsigned long*)(in - blockLen))[0];
            ((unsigned long*)out)[1] = ((unsigned long*)out)[1] ^ ((unsigned long*)(in - blockLen))[1];
            in += blockLen;
            out += blockLen;
            inLen -= blockLen;
        }
        ((unsigned long*)alg->chain)[0] = ((unsigned long*)(in - blockLen))[0];
        ((unsigned long*)alg->chain)[1] = ((unsigned long*)(in - blockLen))[1];
    }
    std::memcpy(alg->buf, in, inLen);
    alg->buffered = (alg->buffered & 0xf0000000) + inLen;
    *outLen -= inLen;
    return 0;
}

static int OFB_DecUpdate(DES_ALG_INFO* alg, unsigned char* in,
                         unsigned long inLen, unsigned char* out,
                         unsigned long* outLen)
{   // ORIG 0x080b0c9f（独立完整体，非转调 OFB_EncUpdate；p167 实证 ORIG 直调
    // DES_Encrypt）。与 EncUpdate 的差别：inLen==blockLen 时保留最后整块交
    // DecFinal 输出（while 条件 `>`），且 buffered 收尾带 0xf0000000 掩码。
    unsigned char* rk = alg->rk;
    unsigned long blockLen = 8;
    unsigned long buffered = alg->buffered;
    *outLen = buffered + inLen;
    if (buffered + inLen <= blockLen) {
        std::memcpy(alg->buf + buffered, in, inLen);
        alg->buffered = alg->buffered + inLen;
        *outLen = 0;
        return 0;
    }
    if (out == in)
        return 0x1001;
    *outLen = buffered + inLen;
    std::memcpy(alg->buf + buffered, in, blockLen - buffered);
    in += blockLen - buffered;
    inLen += buffered - blockLen;
    DES_Encrypt(rk, alg->chain);
    reinterpret_cast<unsigned long*>(out)[0] =
        reinterpret_cast<unsigned long*>(alg->chain)[0] ^
        reinterpret_cast<unsigned long*>(alg->buf)[0];
    reinterpret_cast<unsigned long*>(out)[1] =
        reinterpret_cast<unsigned long*>(alg->chain)[1] ^
        reinterpret_cast<unsigned long*>(alg->buf)[1];
    out += blockLen;
    while (inLen > blockLen) {
        DES_Encrypt(rk, alg->chain);
        reinterpret_cast<unsigned long*>(out)[0] =
            reinterpret_cast<unsigned long*>(alg->chain)[0] ^
            reinterpret_cast<unsigned long*>(in)[0];
        reinterpret_cast<unsigned long*>(out)[1] =
            reinterpret_cast<unsigned long*>(alg->chain)[1] ^
            reinterpret_cast<unsigned long*>(in)[1];
        in += blockLen;
        out += blockLen;
        inLen -= blockLen;
    }
    std::memcpy(alg->buf, in, inLen);
    alg->buffered = (alg->buffered & 0xf0000000) + inLen;
    *outLen -= inLen;
    return 0;
}

static int CFB_DecUpdate(DES_ALG_INFO* alg, unsigned char* in,
                         unsigned long inLen, unsigned char* out,
                         unsigned long* outLen)
{   // 解密：明文 = 密文 ^ E(chain)；链更新为原密文
    unsigned long produced = 0;
    if (alg->buffered > 0) {
        if (alg->buffered + inLen < 8) {
            std::memcpy(alg->buf + alg->buffered, in, inLen);
            alg->buffered += inLen;
            *outLen = 0;
            return 0;
        }
        unsigned long need = 8 - alg->buffered;
        std::memcpy(alg->buf + alg->buffered, in, need);
        DES_Encrypt(alg->rk, alg->chain);
        for (int i = 0; i < 8; ++i) {
            unsigned char plain = (unsigned char)(alg->buf[i] ^ alg->chain[i]);
            alg->chain[i] = alg->buf[i];
            alg->buf[i] = plain;
        }
        std::memcpy(out, alg->buf, 8);
        out += 8;
        produced += 8;
        in += need;
        inLen -= need;
        alg->buffered = 0;
    }
    while (inLen >= 8) {
        unsigned char cipher[8];
        std::memcpy(cipher, in, 8);
        DES_Encrypt(alg->rk, alg->chain);
        for (int i = 0; i < 8; ++i)
            out[i] = (unsigned char)(cipher[i] ^ alg->chain[i]);
        std::memcpy(alg->chain, cipher, 8);
        out += 8;
        in += 8;
        inLen -= 8;
        produced += 8;
    }
    if (inLen > 0) {
        std::memcpy(alg->buf, in, inLen);
        alg->buffered = inLen;
    }
    *outLen = produced;
    return 0;
}

// ---- Enc Final（static；块 8）----

static int ECB_EncFinal(DES_ALG_INFO* alg, unsigned char* out,
                        unsigned long* outLen)
{
    unsigned long len =
        PaddSet(alg->buf, alg->buffered, 8, alg->padding == 2 ? 2 : 1);
    if (len > 8)
        return (int)len;
    if (len == 0 && alg->padding != 2) {
        *outLen = 0;
        return 0;
    }
    DES_Encrypt(alg->rk, alg->buf);
    std::memcpy(out, alg->buf, 8);
    *outLen = 8;
    return 0;
}

static int CBC_EncFinal(DES_ALG_INFO* alg, unsigned char* out,
                        unsigned long* outLen)
{
    unsigned long len = PaddSet(alg->buf, alg->buffered, 8, alg->padding);
    if (len > 8)
        return (int)len;
    if (len == 0) {
        *outLen = 0;
        return 0;
    }
    for (int i = 0; i < 8; ++i)
        alg->buf[i] = (unsigned char)(alg->buf[i] ^ alg->chain[i]);
    DES_Encrypt(alg->rk, alg->buf);
    std::memcpy(out, alg->buf, 8);
    std::memcpy(alg->chain, out, 8);
    *outLen = 8;
    return 0;
}

static int OFB_EncFinal(DES_ALG_INFO* alg, unsigned char* out,
                        unsigned long* outLen)
{
    DES_Encrypt(alg->rk, alg->chain);
    for (unsigned long k = 0; k < alg->buffered; ++k)
        out[k] = (unsigned char)(alg->buf[k] ^ alg->chain[k]);
    *outLen = alg->buffered;
    return 0;
}

static int CFB_EncFinal(DES_ALG_INFO* alg, unsigned char* out,
                        unsigned long* outLen)
{
    DES_Encrypt(alg->rk, alg->chain);
    for (unsigned long k = 0; k < alg->buffered; ++k)
        out[k] = (unsigned char)(alg->buf[k] ^ alg->chain[k]);
    *outLen = alg->buffered;
    return 0;
}

// ---- public 分派：DES ----

#include "ChecksumCrypto/DES_EncUpdate.inc"


#include "ChecksumCrypto/DES_DecUpdate.inc"


#include "ChecksumCrypto/DES_EncFinal.inc"


// ORIG public 四模式 Dec Final（T 符号 0x080b1187..0x080b1403）

#include "ChecksumCrypto/ECB_DecFinal_1.inc"


#include "ChecksumCrypto/CBC_DecFinal_1.inc"


#include "ChecksumCrypto/OFB_DecFinal_1.inc"


#include "ChecksumCrypto/CFB_DecFinal_1.inc"


#include "ChecksumCrypto/DES_DecFinal.inc"


// ---- public 分派：DES3（骨架同 DES，块 8；单块走 3DES-EDE）----

static int ECB_EncUpdate(DES3_ALG_INFO* alg, unsigned char* in,
                         unsigned long inLen, unsigned char* out,
                         unsigned long* outLen)
{
    unsigned long produced = 0;
    if (alg->buffered > 0) {
        if (alg->buffered + inLen < 8) {
            std::memcpy(alg->buf + alg->buffered, in, inLen);
            alg->buffered += inLen;
            *outLen = 0;
            return 0;
        }
        unsigned long need = 8 - alg->buffered;
        std::memcpy(alg->buf + alg->buffered, in, need);
        in += need;
        inLen -= need;
        DES3_Encrypt(alg->rk, alg->buf);
        std::memcpy(out, alg->buf, 8);
        out += 8;
        produced += 8;
        alg->buffered = 0;
    }
    while (inLen >= 8) {
        std::memcpy(out, in, 8);
        DES3_Encrypt(alg->rk, out);
        out += 8;
        in += 8;
        inLen -= 8;
        produced += 8;
    }
    if (inLen > 0) {
        std::memcpy(alg->buf, in, inLen);
        alg->buffered = inLen;
    }
    *outLen = produced;
    return 0;
}

static int CBC_EncUpdate(DES3_ALG_INFO* alg, unsigned char* in,
                         unsigned long inLen, unsigned char* out,
                         unsigned long* outLen)
{
    unsigned long produced = 0;
    if (alg->buffered > 0) {
        if (alg->buffered + inLen < 8) {
            std::memcpy(alg->buf + alg->buffered, in, inLen);
            alg->buffered += inLen;
            *outLen = 0;
            return 0;
        }
        unsigned long need = 8 - alg->buffered;
        std::memcpy(alg->buf + alg->buffered, in, need);
        for (int i = 0; i < 8; ++i)
            alg->buf[i] = (unsigned char)(alg->buf[i] ^ alg->chain[i]);
        DES3_Encrypt(alg->rk, alg->buf);
        std::memcpy(out, alg->buf, 8);
        std::memcpy(alg->chain, alg->buf, 8);
        out += 8;
        produced += 8;
        in += need;
        inLen -= need;
        alg->buffered = 0;
    }
    while (inLen >= 8) {
        for (int i = 0; i < 8; ++i)
            out[i] = (unsigned char)(in[i] ^ alg->chain[i]);
        DES3_Encrypt(alg->rk, out);
        std::memcpy(alg->chain, out, 8);
        out += 8;
        in += 8;
        inLen -= 8;
        produced += 8;
    }
    if (inLen > 0) {
        std::memcpy(alg->buf, in, inLen);
        alg->buffered = inLen;
    }
    *outLen = produced;
    return 0;
}

static int OFB_EncUpdate(DES3_ALG_INFO* alg, unsigned char* in,
                         unsigned long inLen, unsigned char* out,
                         unsigned long* outLen)
{
    unsigned long produced = 0;
    if (alg->buffered > 0) {
        if (alg->buffered + inLen < 8) {
            std::memcpy(alg->buf + alg->buffered, in, inLen);
            alg->buffered += inLen;
            *outLen = 0;
            return 0;
        }
        unsigned long need = 8 - alg->buffered;
        std::memcpy(alg->buf + alg->buffered, in, need);
        DES3_Encrypt(alg->rk, alg->chain);
        for (int i = 0; i < 8; ++i)
            alg->buf[i] = (unsigned char)(alg->buf[i] ^ alg->chain[i]);
        std::memcpy(out, alg->buf, 8);
        out += 8;
        produced += 8;
        in += need;
        inLen -= need;
        alg->buffered = 0;
    }
    while (inLen >= 8) {
        DES3_Encrypt(alg->rk, alg->chain);
        for (int i = 0; i < 8; ++i)
            out[i] = (unsigned char)(in[i] ^ alg->chain[i]);
        out += 8;
        in += 8;
        inLen -= 8;
        produced += 8;
    }
    if (inLen > 0) {
        std::memcpy(alg->buf, in, inLen);
        alg->buffered = inLen;
    }
    *outLen = produced;
    return 0;
}

static int CFB_EncUpdate(DES3_ALG_INFO* alg, unsigned char* in,
                         unsigned long inLen, unsigned char* out,
                         unsigned long* outLen)
{
    unsigned long produced = 0;
    if (alg->buffered > 0) {
        if (alg->buffered + inLen < 8) {
            std::memcpy(alg->buf + alg->buffered, in, inLen);
            alg->buffered += inLen;
            *outLen = 0;
            return 0;
        }
        unsigned long need = 8 - alg->buffered;
        std::memcpy(alg->buf + alg->buffered, in, need);
        DES3_Encrypt(alg->rk, alg->chain);
        for (int i = 0; i < 8; ++i)
            alg->buf[i] = (unsigned char)(alg->buf[i] ^ alg->chain[i]);
        std::memcpy(out, alg->buf, 8);
        std::memcpy(alg->chain, alg->buf, 8);
        out += 8;
        produced += 8;
        in += need;
        inLen -= need;
        alg->buffered = 0;
    }
    while (inLen >= 8) {
        DES3_Encrypt(alg->rk, alg->chain);
        for (int i = 0; i < 8; ++i)
            out[i] = (unsigned char)(in[i] ^ alg->chain[i]);
        std::memcpy(alg->chain, out, 8);
        out += 8;
        in += 8;
        inLen -= 8;
        produced += 8;
    }
    if (inLen > 0) {
        std::memcpy(alg->buf, in, inLen);
        alg->buffered = inLen;
    }
    *outLen = produced;
    return 0;
}

static int ECB_DecUpdate(DES3_ALG_INFO* alg, unsigned char* in,
                         unsigned long inLen, unsigned char* out,
                         unsigned long* outLen)
{
    unsigned long avail = alg->buffered + inLen;
    if (avail < 16) {
        std::memcpy(alg->buf + alg->buffered, in, inLen);
        alg->buffered += inLen;
        *outLen = 0;
        return 0;
    }
    unsigned long produced = 0;
    if (alg->buffered > 0) {
        unsigned long need = 8 - alg->buffered;
        std::memcpy(alg->buf + alg->buffered, in, need);
        DES3_Decrypt(alg->rk, alg->buf);
        std::memcpy(out, alg->buf, 8);
        out += 8;
        produced += 8;
        in += need;
        inLen -= need;
        alg->buffered = 0;
    }
    while (inLen >= 16) {
        std::memcpy(out, in, 8);
        DES3_Decrypt(alg->rk, out);
        out += 8;
        in += 8;
        inLen -= 8;
        produced += 8;
    }
    if (inLen >= 8) {
        std::memcpy(alg->buf, in, inLen);
        alg->buffered = inLen;
    } else if (inLen > 0) {
        std::memcpy(alg->buf + alg->buffered, in, inLen);
        alg->buffered += inLen;
    }
    *outLen = produced;
    return 0;
}

static int CBC_DecUpdate(DES3_ALG_INFO* alg, unsigned char* in,
                         unsigned long inLen, unsigned char* out,
                         unsigned long* outLen)
{   // ORIG 0x080aef1e（CBC 解密独立完整体）：解密后 XOR 前一块密文；
    // 循环条件 `inLen > blockLen` ⇒ 残留 inLen∈[0,blockLen]，尾部 memcpy 不越界
    //（旧实现 `inLen >= 16` + `memcpy(buf,in,inLen)` 在 inLen%8!=0 时越界写 8 字节缓冲）。
    unsigned char* rk = alg->rk;
    unsigned long blockLen = 8;
    unsigned long buffered = alg->buffered;
    *outLen = buffered + inLen;
    if (buffered + inLen <= blockLen) {
        std::memcpy(alg->buf + buffered, in, inLen);
        alg->buffered = alg->buffered + inLen;
        *outLen = 0;
        return 0;
    }
    if (in == out)
        return 0x1001;
    *outLen = buffered + inLen;
    std::memcpy(alg->buf + buffered, in, blockLen - buffered);
    in += blockLen - buffered;
    inLen += buffered - blockLen;
    ((unsigned long*)out)[0] = ((unsigned long*)alg->buf)[0];
    ((unsigned long*)out)[1] = ((unsigned long*)alg->buf)[1];
    DES3_Decrypt(rk, out);
    ((unsigned long*)out)[0] = ((unsigned long*)out)[0] ^ ((unsigned long*)alg->chain)[0];
    ((unsigned long*)out)[1] = ((unsigned long*)out)[1] ^ ((unsigned long*)alg->chain)[1];
    out += blockLen;
    if (inLen <= blockLen) {
        ((unsigned long*)alg->chain)[0] = ((unsigned long*)alg->buf)[0];
        ((unsigned long*)alg->chain)[1] = ((unsigned long*)alg->buf)[1];
    } else {
        if (inLen > blockLen) {
            ((unsigned long*)out)[0] = ((unsigned long*)in)[0];
            ((unsigned long*)out)[1] = ((unsigned long*)in)[1];
            DES3_Decrypt(rk, out);
            ((unsigned long*)out)[0] = ((unsigned long*)out)[0] ^ ((unsigned long*)alg->buf)[0];
            ((unsigned long*)out)[1] = ((unsigned long*)out)[1] ^ ((unsigned long*)alg->buf)[1];
            in += blockLen;
            out += blockLen;
            inLen -= blockLen;
        }
        while (inLen > blockLen) {
            ((unsigned long*)out)[0] = ((unsigned long*)in)[0];
            ((unsigned long*)out)[1] = ((unsigned long*)in)[1];
            DES3_Decrypt(rk, out);
            ((unsigned long*)out)[0] = ((unsigned long*)out)[0] ^ ((unsigned long*)(in - blockLen))[0];
            ((unsigned long*)out)[1] = ((unsigned long*)out)[1] ^ ((unsigned long*)(in - blockLen))[1];
            in += blockLen;
            out += blockLen;
            inLen -= blockLen;
        }
        ((unsigned long*)alg->chain)[0] = ((unsigned long*)(in - blockLen))[0];
        ((unsigned long*)alg->chain)[1] = ((unsigned long*)(in - blockLen))[1];
    }
    std::memcpy(alg->buf, in, inLen);
    alg->buffered = (alg->buffered & 0xf0000000) + inLen;
    *outLen -= inLen;
    return 0;
}

static int OFB_DecUpdate(DES3_ALG_INFO* alg, unsigned char* in,
                         unsigned long inLen, unsigned char* out,
                         unsigned long* outLen)
{   // ORIG 0x080af207（与 DES 版逐条同构，仅 DES3_Encrypt）；独立完整体非转调。
    unsigned char* rk = alg->rk;
    unsigned long blockLen = 8;
    unsigned long buffered = alg->buffered;
    *outLen = buffered + inLen;
    if (buffered + inLen <= blockLen) {
        std::memcpy(alg->buf + buffered, in, inLen);
        alg->buffered = alg->buffered + inLen;
        *outLen = 0;
        return 0;
    }
    if (out == in)
        return 0x1001;
    *outLen = buffered + inLen;
    std::memcpy(alg->buf + buffered, in, blockLen - buffered);
    in += blockLen - buffered;
    inLen += buffered - blockLen;
    DES3_Encrypt(rk, alg->chain);
    reinterpret_cast<unsigned long*>(out)[0] =
        reinterpret_cast<unsigned long*>(alg->chain)[0] ^
        reinterpret_cast<unsigned long*>(alg->buf)[0];
    reinterpret_cast<unsigned long*>(out)[1] =
        reinterpret_cast<unsigned long*>(alg->chain)[1] ^
        reinterpret_cast<unsigned long*>(alg->buf)[1];
    out += blockLen;
    while (inLen > blockLen) {
        DES3_Encrypt(rk, alg->chain);
        reinterpret_cast<unsigned long*>(out)[0] =
            reinterpret_cast<unsigned long*>(alg->chain)[0] ^
            reinterpret_cast<unsigned long*>(in)[0];
        reinterpret_cast<unsigned long*>(out)[1] =
            reinterpret_cast<unsigned long*>(alg->chain)[1] ^
            reinterpret_cast<unsigned long*>(in)[1];
        in += blockLen;
        out += blockLen;
        inLen -= blockLen;
    }
    std::memcpy(alg->buf, in, inLen);
    alg->buffered = (alg->buffered & 0xf0000000) + inLen;
    *outLen -= inLen;
    return 0;
}

static int CFB_DecUpdate(DES3_ALG_INFO* alg, unsigned char* in,
                         unsigned long inLen, unsigned char* out,
                         unsigned long* outLen)
{
    unsigned long produced = 0;
    if (alg->buffered > 0) {
        if (alg->buffered + inLen < 8) {
            std::memcpy(alg->buf + alg->buffered, in, inLen);
            alg->buffered += inLen;
            *outLen = 0;
            return 0;
        }
        unsigned long need = 8 - alg->buffered;
        std::memcpy(alg->buf + alg->buffered, in, need);
        DES3_Encrypt(alg->rk, alg->chain);
        for (int i = 0; i < 8; ++i) {
            unsigned char plain = (unsigned char)(alg->buf[i] ^ alg->chain[i]);
            alg->chain[i] = alg->buf[i];
            alg->buf[i] = plain;
        }
        std::memcpy(out, alg->buf, 8);
        out += 8;
        produced += 8;
        in += need;
        inLen -= need;
        alg->buffered = 0;
    }
    while (inLen >= 8) {
        unsigned char cipher[8];
        std::memcpy(cipher, in, 8);
        DES3_Encrypt(alg->rk, alg->chain);
        for (int i = 0; i < 8; ++i)
            out[i] = (unsigned char)(cipher[i] ^ alg->chain[i]);
        std::memcpy(alg->chain, cipher, 8);
        out += 8;
        in += 8;
        inLen -= 8;
        produced += 8;
    }
    if (inLen > 0) {
        std::memcpy(alg->buf, in, inLen);
        alg->buffered = inLen;
    }
    *outLen = produced;
    return 0;
}

static int ECB_EncFinal(DES3_ALG_INFO* alg, unsigned char* out,
                        unsigned long* outLen)
{
    unsigned long len =
        PaddSet(alg->buf, alg->buffered, 8, alg->padding == 2 ? 2 : 1);
    if (len > 8)
        return (int)len;
    if (len == 0 && alg->padding != 2) {
        *outLen = 0;
        return 0;
    }
    DES3_Encrypt(alg->rk, alg->buf);
    std::memcpy(out, alg->buf, 8);
    *outLen = 8;
    return 0;
}

static int CBC_EncFinal(DES3_ALG_INFO* alg, unsigned char* out,
                        unsigned long* outLen)
{
    unsigned long len = PaddSet(alg->buf, alg->buffered, 8, alg->padding);
    if (len > 8)
        return (int)len;
    if (len == 0) {
        *outLen = 0;
        return 0;
    }
    for (int i = 0; i < 8; ++i)
        alg->buf[i] = (unsigned char)(alg->buf[i] ^ alg->chain[i]);
    DES3_Encrypt(alg->rk, alg->buf);
    std::memcpy(out, alg->buf, 8);
    std::memcpy(alg->chain, out, 8);
    *outLen = 8;
    return 0;
}

static int OFB_EncFinal(DES3_ALG_INFO* alg, unsigned char* out,
                        unsigned long* outLen)
{
    DES3_Encrypt(alg->rk, alg->chain);
    for (unsigned long k = 0; k < alg->buffered; ++k)
        out[k] = (unsigned char)(alg->buf[k] ^ alg->chain[k]);
    *outLen = alg->buffered;
    return 0;
}

static int CFB_EncFinal(DES3_ALG_INFO* alg, unsigned char* out,
                        unsigned long* outLen)
{
    DES3_Encrypt(alg->rk, alg->chain);
    for (unsigned long k = 0; k < alg->buffered; ++k)
        out[k] = (unsigned char)(alg->buf[k] ^ alg->chain[k]);
    *outLen = alg->buffered;
    return 0;
}

#include "ChecksumCrypto/DES3_EncUpdate.inc"


#include "ChecksumCrypto/DES3_DecUpdate.inc"


#include "ChecksumCrypto/DES3_EncFinal.inc"


// ORIG public 3DES 四模式 Dec Final（T 符号 0x080af6ef..0x080af96b）

#include "ChecksumCrypto/ECB_DecFinal_2.inc"


#include "ChecksumCrypto/CBC_DecFinal_2.inc"


#include "ChecksumCrypto/OFB_DecFinal_2.inc"


#include "ChecksumCrypto/CFB_DecFinal_2.inc"


#include "ChecksumCrypto/DES3_DecFinal.inc"


// ---------------------------------------------------------------------------
// CRijndael_fake（ORIG 0x080b824c..0x080b851c）
// 类布局：vptr@0x00、m_pCtxObf@0x04（存 new ptr ^ m_salt）、m_salt@0x08（ctor 随机）。
// 实际使用 Anubis-128/12轮 冒充 Rijndael（"fake"之名由此）——按 ORIG 忠实转写。
// ---------------------------------------------------------------------------

#define fakeXor(p, salt) \
    reinterpret_cast<unsigned char*>(reinterpret_cast<std::size_t>(p) ^ (std::size_t)(salt))

#include "ChecksumCrypto/CRijndael_fake.inc"


#include "ChecksumCrypto/dtor_CRijndael_fake.inc"


#include "ChecksumCrypto/Initialize_4.inc"


#include "ChecksumCrypto/Encrypt.inc"


#include "ChecksumCrypto/Decrypt.inc"


#include "ChecksumCrypto/Pad.inc"


#include "ChecksumCrypto/GetBlockSize.inc"


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

// ---------------------------------------------------------------------------
// CSHA —— 自实现 SHA-256（ORIG 0x080bb69c..0x080bd977）
// 报告：docs/class_func_reports/CSHA/{CSHA,AddData,FinalDigest,Reset,Transform}.md
// 布局：state[8]@0x00, m_lenLo@0x20, m_lenHi@0x24, m_buffer[64]@0x28,
//       m_pending@0x68（AddData/FinalDigest 残留标志，ORIG 行为照抄）。
// 算法为 FIPS 180-4 标准 SHA-256（用户规则豁免 identical，语义真实现）。
// ---------------------------------------------------------------------------

unsigned int const CSHA::sm_H256[8] = {
    0x6a09e667u, 0xbb67ae85u, 0x3c6ef372u, 0xa54ff53au,
    0x510e527fu, 0x9b05688cu, 0x1f83d9abu, 0x5be0cd19u,
};  // ORIG CSHA::sm_H256 @0x8b230e0（小端存储）

unsigned int const CSHA::sm_K256[64] = {
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
    0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u,
};  // ORIG CSHA::sm_K256 @0x8b22fe0

#include "ChecksumCrypto/CSHA.inc"


#include "ChecksumCrypto/AddData.inc"


#include "ChecksumCrypto/FinalDigest.inc"


#include "ChecksumCrypto/Reset.inc"


#include "ChecksumCrypto/Transform.inc"



// ---------------------------------------------------------------------------
// CSHA1 —— 经典 SHA-1（ORIG 0x08087d28..0x0808a4d5）。
// 报告：docs/class_func_reports/CSHA1/{CSHA1,Reset,Transform,Update,Final,
// GetHash,GetHexHash,~CSHA1}.md。算法类语义正确实现（用户规则：算法族
// 不要求 identical）；行为与 ORIG 报告逐一对应。
// ---------------------------------------------------------------------------

#include "ChecksumCrypto/CSHA1.inc"


#include "ChecksumCrypto/dtor_CSHA1.inc"


#include "ChecksumCrypto/Reset_1.inc"


#include "ChecksumCrypto/Update.inc"


#include "ChecksumCrypto/Transform_1.inc"


#include "ChecksumCrypto/Final.inc"


#include "ChecksumCrypto/GetHash.inc"


#include "ChecksumCrypto/GetHexHash.inc"

