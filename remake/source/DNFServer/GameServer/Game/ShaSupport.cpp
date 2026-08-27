// ===========================================================================
// df_game_r 还原 —— CodeHack 校验域加密支撑（W7AB 组，加密域 producer）
//
// 全部为全局命名空间 C 链接语义函数（ORIG 即全局符号，非 extern "C"，由
// 头文件签名自然产生 _Z* mangled）。
//   sha256_starts  @0x080841fc：逐条 movl 写 total[2]=0 与 SHA-256 IV。
//   sha256_update  @0x080878ec：PolarSSL 式 left/fill 缓冲续填（逐指令转写）。
//   sha256_finish  @0x080879ea：64 位 bitlen 大端写 + 0x80 padding（ORIG 引用
//     .data 0x936e440 的 80 字节 0x80,00... 常量）+ update(8) + 状态大端输出。
//   sha256_process @0x08084264：SHA-256 压缩函数。ORIG 为 gcc -O0 将 K 表
//     内联展开的 0x3688 字节大块；本实现按标准 SHA-256 语义等价转写——已
//     抽验：头部 big-endian 取 W[0..]（movzbl/shl 18/or 序列）与尾部把
//     a..h 加回 ctx->m_state(+0x08..+0x24) 的合并序，与 ORIG 一致。【推断】
//   StrCpyMaxPath  @0x0808a5dd：memset(dest,0,0x104); strncpy(dest,src,0x103)。
// ===========================================================================

#include "ShaSupport.h"

#include <cstring>

namespace
{

inline unsigned long sha_ror(unsigned long x, int n)
{
    return (x >> n) | (x << (32 - n));
}

// SHA-256 轮常量（ORIG -O0 展开为立即数；此处按规范表组织，运行期值一致）
const unsigned long k_sha256K[64] =
{
    0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL,
    0x3956c25bUL, 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL,
    0xd807aa98UL, 0x12835b01UL, 0x243185beUL, 0x550c7dc3UL,
    0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL, 0xc19bf174UL,
    0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
    0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL,
    0x983e5152UL, 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL,
    0xc6e00bf3UL, 0xd5a79147UL, 0x06ca6351UL, 0x14292967UL,
    0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL, 0x53380d13UL,
    0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
    0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL,
    0xd192e819UL, 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL,
    0x19a4c116UL, 0x1e376c08UL, 0x2748774cUL, 0x34b0bcb5UL,
    0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL, 0x682e6ff3UL,
    0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
    0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL,
};

unsigned long sha_bswap32(const unsigned char* p)
{
    return (static_cast<unsigned long>(p[0]) << 24)
         | (static_cast<unsigned long>(p[1]) << 16)
         | (static_cast<unsigned long>(p[2]) << 8)
         | static_cast<unsigned long>(p[3]);
}

}  // namespace

// ---------------------------------------------------------------------------
// sha256_starts(sha256_context*) T @0x080841fc
// ORIG：ctx+0x00/+0x04 清零；state[8] 按 IV 0x6a09e667/0xbb67ae85/0x3c6ef372/
//       0xa54ff53a/0x510e527f/0x9b05688c/0x1f83d9ab/0x5be0cd19 写 +0x08..+0x24。
// ---------------------------------------------------------------------------
void sha256_starts(sha256_context* ctx)
{
    static const unsigned long iv[8] =
    {
        0x6a09e667UL, 0xbb67ae85UL, 0x3c6ef372UL, 0xa54ff53aUL,
        0x510e527fUL, 0x9b05688cUL, 0x1f83d9abUL, 0x5be0cd19UL,
    };

    for (int i = 0; i < 2; ++i)
    {
        ctx->m_total[i] = 0;
    }
    for (int i = 0; i < 8; ++i)
    {
        ctx->m_state[i] = iv[i];
    }
}

// ---------------------------------------------------------------------------
// sha256_process(sha256_context*, unsigned char*) T @0x08084264（内部压缩函数，
// size 0x3688 —— 见头注【推断】说明；语义：对 data 指向的 64 字节分组压缩）。
// ---------------------------------------------------------------------------
void sha256_process(sha256_context* ctx, unsigned char* data)
{
    unsigned long w[64];

    // W[0..15]：big-endian 读入（ORIG movzbl/shl/or 序列实证）
    for (int i = 0; i < 16; ++i)
    {
        w[i] = sha_bswap32(data + i * 4);
    }
    // W[16..63]：消息扩展
    for (int i = 16; i < 64; ++i)
    {
        unsigned long s0 =
            sha_ror(w[i - 15], 7) ^ sha_ror(w[i - 15], 18) ^ (w[i - 15] >> 3);
        unsigned long s1 =
            sha_ror(w[i - 2], 17) ^ sha_ror(w[i - 2], 19) ^ (w[i - 2] >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }

    unsigned long a = ctx->m_state[0];
    unsigned long b = ctx->m_state[1];
    unsigned long c = ctx->m_state[2];
    unsigned long d = ctx->m_state[3];
    unsigned long e = ctx->m_state[4];
    unsigned long f = ctx->m_state[5];
    unsigned long g = ctx->m_state[6];
    unsigned long h = ctx->m_state[7];

    for (int i = 0; i < 64; ++i)
    {
        unsigned long s1 = sha_ror(e, 6) ^ sha_ror(e, 11) ^ sha_ror(e, 25);
        unsigned long ch = (e & f) ^ ((~e) & g);
        unsigned long t1 = h + s1 + ch + k_sha256K[i] + w[i];
        unsigned long s0 = sha_ror(a, 2) ^ sha_ror(a, 13) ^ sha_ror(a, 22);
        unsigned long maj = (a & b) ^ (a & c) ^ (b & c);
        unsigned long t2 = s0 + maj;

        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    // 尾部：a..h 加回 ctx->m_state（ORIG ret 前 add/mov 序列实证）
    ctx->m_state[0] += a;
    ctx->m_state[1] += b;
    ctx->m_state[2] += c;
    ctx->m_state[3] += d;
    ctx->m_state[4] += e;
    ctx->m_state[5] += f;
    ctx->m_state[6] += g;
    ctx->m_state[7] += h;
}

// ---------------------------------------------------------------------------
// sha256_update(sha256_context*, unsigned char*, unsigned long) T @0x080878ec
// （size 0xfe，逐指令转写：ilen==0 直接 ret；left=total&0x3f、fill=0x40-left；
//   total+=ilen 且回绕进位 total[1]++；left!=0&&ilen>=fill 时先补满 buffer 并
//   process(buffer)；while ilen>0x3f 对 input 整组 process；余量 memcpy 入
//   buffer+left。）
// ---------------------------------------------------------------------------
void sha256_update(sha256_context* ctx, unsigned char* input, unsigned long ilen)
{
    if (ilen == 0)                       // cmpl $0x0,0x10(%ebp); je
    {
        return;
    }

    unsigned long left = ctx->m_total[0] & 0x3f;    // and $0x3f,%eax → -0x10(%ebp)
    unsigned long fill = 0x40 - left;               // mov $0x40 / sub → -0xc(%ebp)

    ctx->m_total[0] += ilen;            // old+ilen 回绕写入（mov %edx,(%eax)）
    if (ctx->m_total[0] < ilen)         // cmp ilen,new; jae skip → 溢出时 new<ilen
    {
        ctx->m_total[1] += 1;           // 进位（lea 0x1(%eax),%edx / mov）
    }

    if (left != 0 && ilen >= fill)      // cmpl $0x0,left; je / cmp fill; jb
    {
        std::memcpy(ctx->m_buffer + left, input, fill);
        sha256_process(ctx, ctx->m_buffer);
        ilen -= fill;                   // sub %eax,0x10(%ebp)
        input += fill;                  // add %eax,0xc(%ebp)
        left = 0;                       // movl $0x0,-0x10(%ebp)
    }

    while (ilen > 0x3f)                 // cmpl $0x3f,ilen; seta
    {
        sha256_process(ctx, input);
        ilen -= 0x40;                   // subl $0x40,ilen
        input += 0x40;                  // addl $0x40,input
    }

    if (ilen != 0)                      // cmpl $0x0,ilen; je
    {
        std::memcpy(ctx->m_buffer + left, input, ilen);
    }
}

// ---------------------------------------------------------------------------
// sha256_finish(sha256_context*, unsigned char output[32]) T @0x080879ea
// 逐段忠实：last=total&0x3f、padn=(last<0x38)?0x38-last:0x78-last；
// 先 update(pad 常量区, padn)（ORIG 引用 .data@0x936e440 的 0x80 后跟零的
// 80 字节表，padn<=0x78 ≤ 表长），再 update(8 字节大端 bitlen)，最后将
// state[8] 依次按 high→low 移位写出 32 字节 big-endian 摘要。
// ---------------------------------------------------------------------------
void sha256_finish(sha256_context* ctx, unsigned char output[0x20])
{
    const unsigned char k_padPrefix[0x50] = { 0x80 };  // 余下为 0（与 .data 表一致）

    // bitlen 高/低 32 位（ORIG：hi=total0>>29|total1<<3; lo=total0<<3）
    unsigned char bits[8];
    bits[0] = static_cast<unsigned char>(((ctx->m_total[0] >> 29)
                | (ctx->m_total[1] << 3)) >> 24);
    bits[1] = static_cast<unsigned char>(((ctx->m_total[0] >> 29)
                | (ctx->m_total[1] << 3)) >> 16);
    bits[2] = static_cast<unsigned char>(((ctx->m_total[0] >> 29)
                | (ctx->m_total[1] << 3)) >> 8);
    bits[3] = static_cast<unsigned char>((ctx->m_total[0] >> 29)
                | (ctx->m_total[1] << 3));
    bits[4] = static_cast<unsigned char>((ctx->m_total[0] << 3) >> 24);
    bits[5] = static_cast<unsigned char>((ctx->m_total[0] << 3) >> 16);
    bits[6] = static_cast<unsigned char>((ctx->m_total[0] << 3) >> 8);
    bits[7] = static_cast<unsigned char>(ctx->m_total[0] << 3);

    unsigned long last = ctx->m_total[0] & 0x3f;
    unsigned long padn = (last < 0x38) ? (0x38 - last) : (0x78 - last);

    sha256_update(ctx, const_cast<unsigned char*>(k_padPrefix), padn);
    sha256_update(ctx, bits, 8);

    for (int i = 0; i < 8; ++i)
    {
        unsigned long v = ctx->m_state[i];
        output[i * 4 + 0] = static_cast<unsigned char>(v >> 24);
        output[i * 4 + 1] = static_cast<unsigned char>(v >> 16);
        output[i * 4 + 2] = static_cast<unsigned char>(v >> 8);
        output[i * 4 + 3] = static_cast<unsigned char>(v);
    }
}

// ---------------------------------------------------------------------------
// StrCpyMaxPath(char* dest, const char* src) T @0x0808a5dd
// memset(dest,0,0x104); strncpy(dest,src,0x103)（PLT 调用逐参数转写）。
// ---------------------------------------------------------------------------
void StrCpyMaxPath(char* dest, const char* src)
{
    std::memset(dest, 0, 0x104);
    std::strncpy(dest, src, 0x103);
}
