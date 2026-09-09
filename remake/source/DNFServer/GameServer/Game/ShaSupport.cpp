// ===========================================================================
// df_game_r 还原 —— CodeHack 校验域加密支撑（W7AB 组，加密域 producer）
//
// 全部为全局命名空间 C 链接语义函数（ORIG 即全局符号，非 extern "C"，由
// 头文件签名自然产生 _Z* mangled）。
//   sha256_starts  @0x080841fc：逐条 movl 写 total[2]=0 与 SHA-256 IV。
//   sha256_update  @0x080878ec：PolarSSL 式 left/fill 缓冲续填（逐指令转写）。
//   sha256_finish  @0x080879ea：64 位 bitlen 大端写 + 0x80 padding（ORIG 引用
//     .data 0x936e440 的 80 字节 0x80,00... 常量）+ update(8) + 状态大端输出。
//   sha256_process @0x08084264：SHA-256 压缩函数（size 0x3688）。b28d-solo 起
//     改为 libtomcrypt 风格全展开（W 表 LOAD32H 16 组 + 64 轮 RND 宏 + K 立即
//     数 + W[i>=16] 轮内内联扩展），与 ORIG 逐条同构；残差见函数头判词（N）。
//   StrCpyMaxPath  @0x0808a5dd：memset(dest,0,0x104); strncpy(dest,src,0x103)。
// ===========================================================================

#include "ShaSupport.h"

#include <cstring>


// ---------------------------------------------------------------------------
// sha256_starts(sha256_context*) T @0x080841fc
// ORIG：ctx+0x00/+0x04 清零；state[8] 按 IV 0x6a09e667/0xbb67ae85/0x3c6ef372/
//       0xa54ff53a/0x510e527f/0x9b05688c/0x1f83d9ab/0x5be0cd19 写 +0x08..+0x24。
// [b28d-L1] 修法：ORIG 0x080841fc..0x08084263 为**逐条展开**的 10 条独立赋值
// （每条 `mov 0x8(%ebp),%eax` 重载 + store，无循环/无 iv 表）；原循环形使 4.4
// 发 sub/setle/回边（27 条 vs ORIG 24 条 DIFF）。展开为逐字段赋值后逐条对齐
// （P168 表达式直读：每处重载 this，不缓存 ctx 指针）。
// ---------------------------------------------------------------------------
void sha256_starts(sha256_context* ctx)
{
    ctx->m_total[0] = 0;
    ctx->m_total[1] = 0;
    ctx->m_state[0] = 0x6a09e667UL;
    ctx->m_state[1] = 0xbb67ae85UL;
    ctx->m_state[2] = 0x3c6ef372UL;
    ctx->m_state[3] = 0xa54ff53aUL;
    ctx->m_state[4] = 0x510e527fUL;
    ctx->m_state[5] = 0x9b05688cUL;
    ctx->m_state[6] = 0x1f83d9abUL;
    ctx->m_state[7] = 0x5be0cd19UL;
}

// ---------------------------------------------------------------------------
// sha256_process(sha256_context*, unsigned char*) T @0x08084264（内部压缩函数，
// size 0x3688）。ORIG 为 libtomcrypt 风格**全展开**：W[0..15] 逐条 big-endian
// 组装（槽 -0x12c..-0xf0）、S[0..7] 逐条载入（槽 -0x24..-0x8）、64 轮 RND 宏展开
// （参数槽每轮轮转，W[i>=16] 在轮表达式内联扩展）、尾部 8 条 state 累加。
// 探针 /tmp/probe_ror.cpp 实证 4.4.7 与本形同构：`(x>>n)|(x<<(32-n))` 发硬件
// `ror`（ORIG 同）、Ch/Maj/`t+w+K`（lea+add/sub imm）逐条同形。
// [b28d-solo-ShaSupport] 修法：删除匿名 namespace 的 sha_ror/sha_bswap32 helper
// 与 k_sha256K[64] 表——ORIG nm 无此三符号（=发明符号），p167 原报 EXTRA 11 个
// helper call；改为宏 + K 立即数全展开后调用点差集 0/0。循环抽象 → 展开形。
// [b28d-solo-ShaSupport] 判词 N 封顶（4.4 内存源操作数限制，N86b 族）：
// ORIG 4687 条 vs 我方 4945 条（gap +258 = 64 轮 × 4 + push/pop %esi 2 条）。
// 4.4 -O0 对 `reg op mem` 恒先 `mov mem,%reg`（/tmp/probe_and.cpp a1..a5、
// /tmp/probe_ch.cpp cA..cE 八形实证：`and/xor/add mem,reg` 不可得），ORIG 4.1
// 每轮 Ch 5 条（2 mov + 3 内存源 op）vs 我方 7 条、Maj 同差；连带 T1 中间值需
// %esi ⇒ 全帧 disp +4。语义闭合：p167 CLEAR 0/0 + /tmp/sha_test.cpp 五向量
// （abc/448bit/empty/million-a/fox-bytewise）全 OK。§6 算法类豁免并存。
// ---------------------------------------------------------------------------
#define SHA256_ROR(x, n) (((x) >> (n)) | ((x) << (32 - (n))))
#define SHA256_CH(x, y, z) ((z) ^ ((x) & ((y) ^ (z))))
#define SHA256_MAJ(x, y, z) (((x) & (y)) | (((x) | (y)) & (z)))
#define SHA256_BSIG0(x) (SHA256_ROR((x), 2) ^ SHA256_ROR((x), 13) ^ SHA256_ROR((x), 22))
#define SHA256_BSIG1(x) (SHA256_ROR((x), 6) ^ SHA256_ROR((x), 11) ^ SHA256_ROR((x), 25))
#define SHA256_SSIG0(x) (SHA256_ROR((x), 7) ^ SHA256_ROR((x), 18) ^ ((x) >> 3))
#define SHA256_SSIG1(x) (SHA256_ROR((x), 17) ^ SHA256_ROR((x), 19) ^ ((x) >> 10))
#define SHA256_RND(a, b, c, d, e, f, g, h, i, ki)                      \
    do {                                                               \
        t0 = SHA256_BSIG1(e) + (h) + SHA256_CH(e, f, g) + w[i] + (ki); \
        t1 = SHA256_BSIG0(a) + SHA256_MAJ(a, b, c); \
        (d) += t0;                                                     \
        (h) = t0 + t1;                                                 \
    } while (0)
#define SHA256_RNDX(a, b, c, d, e, f, g, h, i, ki)                     \
    do {                                                               \
        w[i] = SHA256_SSIG1(w[i - 2]) + w[i - 7]                       \
             + SHA256_SSIG0(w[i - 15]) + w[i - 16];                    \
        SHA256_RND(a, b, c, d, e, f, g, h, i, ki);                     \
    } while (0)

void sha256_process(sha256_context* ctx, unsigned char* data)
{
    unsigned long w[64];
    unsigned long t0;
    unsigned long t1;
    unsigned long s[8];
    w[0] = ((unsigned long)(data[0] & 255) << 24)
           | ((unsigned long)(data[1] & 255) << 16)
           | ((unsigned long)(data[2] & 255) << 8)
           | (unsigned long)(data[3] & 255);
    w[1] = ((unsigned long)(data[4] & 255) << 24)
           | ((unsigned long)(data[5] & 255) << 16)
           | ((unsigned long)(data[6] & 255) << 8)
           | (unsigned long)(data[7] & 255);
    w[2] = ((unsigned long)(data[8] & 255) << 24)
           | ((unsigned long)(data[9] & 255) << 16)
           | ((unsigned long)(data[10] & 255) << 8)
           | (unsigned long)(data[11] & 255);
    w[3] = ((unsigned long)(data[12] & 255) << 24)
           | ((unsigned long)(data[13] & 255) << 16)
           | ((unsigned long)(data[14] & 255) << 8)
           | (unsigned long)(data[15] & 255);
    w[4] = ((unsigned long)(data[16] & 255) << 24)
           | ((unsigned long)(data[17] & 255) << 16)
           | ((unsigned long)(data[18] & 255) << 8)
           | (unsigned long)(data[19] & 255);
    w[5] = ((unsigned long)(data[20] & 255) << 24)
           | ((unsigned long)(data[21] & 255) << 16)
           | ((unsigned long)(data[22] & 255) << 8)
           | (unsigned long)(data[23] & 255);
    w[6] = ((unsigned long)(data[24] & 255) << 24)
           | ((unsigned long)(data[25] & 255) << 16)
           | ((unsigned long)(data[26] & 255) << 8)
           | (unsigned long)(data[27] & 255);
    w[7] = ((unsigned long)(data[28] & 255) << 24)
           | ((unsigned long)(data[29] & 255) << 16)
           | ((unsigned long)(data[30] & 255) << 8)
           | (unsigned long)(data[31] & 255);
    w[8] = ((unsigned long)(data[32] & 255) << 24)
           | ((unsigned long)(data[33] & 255) << 16)
           | ((unsigned long)(data[34] & 255) << 8)
           | (unsigned long)(data[35] & 255);
    w[9] = ((unsigned long)(data[36] & 255) << 24)
           | ((unsigned long)(data[37] & 255) << 16)
           | ((unsigned long)(data[38] & 255) << 8)
           | (unsigned long)(data[39] & 255);
    w[10] = ((unsigned long)(data[40] & 255) << 24)
           | ((unsigned long)(data[41] & 255) << 16)
           | ((unsigned long)(data[42] & 255) << 8)
           | (unsigned long)(data[43] & 255);
    w[11] = ((unsigned long)(data[44] & 255) << 24)
           | ((unsigned long)(data[45] & 255) << 16)
           | ((unsigned long)(data[46] & 255) << 8)
           | (unsigned long)(data[47] & 255);
    w[12] = ((unsigned long)(data[48] & 255) << 24)
           | ((unsigned long)(data[49] & 255) << 16)
           | ((unsigned long)(data[50] & 255) << 8)
           | (unsigned long)(data[51] & 255);
    w[13] = ((unsigned long)(data[52] & 255) << 24)
           | ((unsigned long)(data[53] & 255) << 16)
           | ((unsigned long)(data[54] & 255) << 8)
           | (unsigned long)(data[55] & 255);
    w[14] = ((unsigned long)(data[56] & 255) << 24)
           | ((unsigned long)(data[57] & 255) << 16)
           | ((unsigned long)(data[58] & 255) << 8)
           | (unsigned long)(data[59] & 255);
    w[15] = ((unsigned long)(data[60] & 255) << 24)
           | ((unsigned long)(data[61] & 255) << 16)
           | ((unsigned long)(data[62] & 255) << 8)
           | (unsigned long)(data[63] & 255);

    s[0] = ctx->m_state[0];
    s[1] = ctx->m_state[1];
    s[2] = ctx->m_state[2];
    s[3] = ctx->m_state[3];
    s[4] = ctx->m_state[4];
    s[5] = ctx->m_state[5];
    s[6] = ctx->m_state[6];
    s[7] = ctx->m_state[7];

    SHA256_RND (s[0],s[1],s[2],s[3],s[4],s[5],s[6],s[7], 0, 0x428a2f98UL);
    SHA256_RND (s[7],s[0],s[1],s[2],s[3],s[4],s[5],s[6], 1, 0x71374491UL);
    SHA256_RND (s[6],s[7],s[0],s[1],s[2],s[3],s[4],s[5], 2, 0xb5c0fbcfUL);
    SHA256_RND (s[5],s[6],s[7],s[0],s[1],s[2],s[3],s[4], 3, 0xe9b5dba5UL);
    SHA256_RND (s[4],s[5],s[6],s[7],s[0],s[1],s[2],s[3], 4, 0x3956c25bUL);
    SHA256_RND (s[3],s[4],s[5],s[6],s[7],s[0],s[1],s[2], 5, 0x59f111f1UL);
    SHA256_RND (s[2],s[3],s[4],s[5],s[6],s[7],s[0],s[1], 6, 0x923f82a4UL);
    SHA256_RND (s[1],s[2],s[3],s[4],s[5],s[6],s[7],s[0], 7, 0xab1c5ed5UL);
    SHA256_RND (s[0],s[1],s[2],s[3],s[4],s[5],s[6],s[7], 8, 0xd807aa98UL);
    SHA256_RND (s[7],s[0],s[1],s[2],s[3],s[4],s[5],s[6], 9, 0x12835b01UL);
    SHA256_RND (s[6],s[7],s[0],s[1],s[2],s[3],s[4],s[5], 10, 0x243185beUL);
    SHA256_RND (s[5],s[6],s[7],s[0],s[1],s[2],s[3],s[4], 11, 0x550c7dc3UL);
    SHA256_RND (s[4],s[5],s[6],s[7],s[0],s[1],s[2],s[3], 12, 0x72be5d74UL);
    SHA256_RND (s[3],s[4],s[5],s[6],s[7],s[0],s[1],s[2], 13, 0x80deb1feUL);
    SHA256_RND (s[2],s[3],s[4],s[5],s[6],s[7],s[0],s[1], 14, 0x9bdc06a7UL);
    SHA256_RND (s[1],s[2],s[3],s[4],s[5],s[6],s[7],s[0], 15, 0xc19bf174UL);
    SHA256_RNDX(s[0],s[1],s[2],s[3],s[4],s[5],s[6],s[7], 16, 0xe49b69c1UL);
    SHA256_RNDX(s[7],s[0],s[1],s[2],s[3],s[4],s[5],s[6], 17, 0xefbe4786UL);
    SHA256_RNDX(s[6],s[7],s[0],s[1],s[2],s[3],s[4],s[5], 18, 0x0fc19dc6UL);
    SHA256_RNDX(s[5],s[6],s[7],s[0],s[1],s[2],s[3],s[4], 19, 0x240ca1ccUL);
    SHA256_RNDX(s[4],s[5],s[6],s[7],s[0],s[1],s[2],s[3], 20, 0x2de92c6fUL);
    SHA256_RNDX(s[3],s[4],s[5],s[6],s[7],s[0],s[1],s[2], 21, 0x4a7484aaUL);
    SHA256_RNDX(s[2],s[3],s[4],s[5],s[6],s[7],s[0],s[1], 22, 0x5cb0a9dcUL);
    SHA256_RNDX(s[1],s[2],s[3],s[4],s[5],s[6],s[7],s[0], 23, 0x76f988daUL);
    SHA256_RNDX(s[0],s[1],s[2],s[3],s[4],s[5],s[6],s[7], 24, 0x983e5152UL);
    SHA256_RNDX(s[7],s[0],s[1],s[2],s[3],s[4],s[5],s[6], 25, 0xa831c66dUL);
    SHA256_RNDX(s[6],s[7],s[0],s[1],s[2],s[3],s[4],s[5], 26, 0xb00327c8UL);
    SHA256_RNDX(s[5],s[6],s[7],s[0],s[1],s[2],s[3],s[4], 27, 0xbf597fc7UL);
    SHA256_RNDX(s[4],s[5],s[6],s[7],s[0],s[1],s[2],s[3], 28, 0xc6e00bf3UL);
    SHA256_RNDX(s[3],s[4],s[5],s[6],s[7],s[0],s[1],s[2], 29, 0xd5a79147UL);
    SHA256_RNDX(s[2],s[3],s[4],s[5],s[6],s[7],s[0],s[1], 30, 0x06ca6351UL);
    SHA256_RNDX(s[1],s[2],s[3],s[4],s[5],s[6],s[7],s[0], 31, 0x14292967UL);
    SHA256_RNDX(s[0],s[1],s[2],s[3],s[4],s[5],s[6],s[7], 32, 0x27b70a85UL);
    SHA256_RNDX(s[7],s[0],s[1],s[2],s[3],s[4],s[5],s[6], 33, 0x2e1b2138UL);
    SHA256_RNDX(s[6],s[7],s[0],s[1],s[2],s[3],s[4],s[5], 34, 0x4d2c6dfcUL);
    SHA256_RNDX(s[5],s[6],s[7],s[0],s[1],s[2],s[3],s[4], 35, 0x53380d13UL);
    SHA256_RNDX(s[4],s[5],s[6],s[7],s[0],s[1],s[2],s[3], 36, 0x650a7354UL);
    SHA256_RNDX(s[3],s[4],s[5],s[6],s[7],s[0],s[1],s[2], 37, 0x766a0abbUL);
    SHA256_RNDX(s[2],s[3],s[4],s[5],s[6],s[7],s[0],s[1], 38, 0x81c2c92eUL);
    SHA256_RNDX(s[1],s[2],s[3],s[4],s[5],s[6],s[7],s[0], 39, 0x92722c85UL);
    SHA256_RNDX(s[0],s[1],s[2],s[3],s[4],s[5],s[6],s[7], 40, 0xa2bfe8a1UL);
    SHA256_RNDX(s[7],s[0],s[1],s[2],s[3],s[4],s[5],s[6], 41, 0xa81a664bUL);
    SHA256_RNDX(s[6],s[7],s[0],s[1],s[2],s[3],s[4],s[5], 42, 0xc24b8b70UL);
    SHA256_RNDX(s[5],s[6],s[7],s[0],s[1],s[2],s[3],s[4], 43, 0xc76c51a3UL);
    SHA256_RNDX(s[4],s[5],s[6],s[7],s[0],s[1],s[2],s[3], 44, 0xd192e819UL);
    SHA256_RNDX(s[3],s[4],s[5],s[6],s[7],s[0],s[1],s[2], 45, 0xd6990624UL);
    SHA256_RNDX(s[2],s[3],s[4],s[5],s[6],s[7],s[0],s[1], 46, 0xf40e3585UL);
    SHA256_RNDX(s[1],s[2],s[3],s[4],s[5],s[6],s[7],s[0], 47, 0x106aa070UL);
    SHA256_RNDX(s[0],s[1],s[2],s[3],s[4],s[5],s[6],s[7], 48, 0x19a4c116UL);
    SHA256_RNDX(s[7],s[0],s[1],s[2],s[3],s[4],s[5],s[6], 49, 0x1e376c08UL);
    SHA256_RNDX(s[6],s[7],s[0],s[1],s[2],s[3],s[4],s[5], 50, 0x2748774cUL);
    SHA256_RNDX(s[5],s[6],s[7],s[0],s[1],s[2],s[3],s[4], 51, 0x34b0bcb5UL);
    SHA256_RNDX(s[4],s[5],s[6],s[7],s[0],s[1],s[2],s[3], 52, 0x391c0cb3UL);
    SHA256_RNDX(s[3],s[4],s[5],s[6],s[7],s[0],s[1],s[2], 53, 0x4ed8aa4aUL);
    SHA256_RNDX(s[2],s[3],s[4],s[5],s[6],s[7],s[0],s[1], 54, 0x5b9cca4fUL);
    SHA256_RNDX(s[1],s[2],s[3],s[4],s[5],s[6],s[7],s[0], 55, 0x682e6ff3UL);
    SHA256_RNDX(s[0],s[1],s[2],s[3],s[4],s[5],s[6],s[7], 56, 0x748f82eeUL);
    SHA256_RNDX(s[7],s[0],s[1],s[2],s[3],s[4],s[5],s[6], 57, 0x78a5636fUL);
    SHA256_RNDX(s[6],s[7],s[0],s[1],s[2],s[3],s[4],s[5], 58, 0x84c87814UL);
    SHA256_RNDX(s[5],s[6],s[7],s[0],s[1],s[2],s[3],s[4], 59, 0x8cc70208UL);
    SHA256_RNDX(s[4],s[5],s[6],s[7],s[0],s[1],s[2],s[3], 60, 0x90befffaUL);
    SHA256_RNDX(s[3],s[4],s[5],s[6],s[7],s[0],s[1],s[2], 61, 0xa4506cebUL);
    SHA256_RNDX(s[2],s[3],s[4],s[5],s[6],s[7],s[0],s[1], 62, 0xbef9a3f7UL);
    SHA256_RNDX(s[1],s[2],s[3],s[4],s[5],s[6],s[7],s[0], 63, 0xc67178f2UL);

    ctx->m_state[0] += s[0];
    ctx->m_state[1] += s[1];
    ctx->m_state[2] += s[2];
    ctx->m_state[3] += s[3];
    ctx->m_state[4] += s[4];
    ctx->m_state[5] += s[5];
    ctx->m_state[6] += s[6];
    ctx->m_state[7] += s[7];
}

#undef SHA256_RNDX
#undef SHA256_RND
#undef SHA256_SSIG1
#undef SHA256_SSIG0
#undef SHA256_BSIG1
#undef SHA256_BSIG0
#undef SHA256_MAJ
#undef SHA256_CH
#undef SHA256_ROR

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
    ctx->m_total[0] = ctx->m_total[0];  // ORIG 冗余 reload/store 对（0x08087921-29，无观察效应）
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
// [b28d-solo-ShaSupport] 修法：ORIG 0x080879ea 为**全展开**形——文件级
//   static pad 表（nm 实证 `_ZL14sha256_padding` .data size 0x40）+ 具名
//   hi/lo 槽（-0x10/-0xc，每处 bits 赋值前重读槽 = P0 槽往返）+ bits[8]
//   （-0x20..-0x19）+ last/padn（-0x18/-0x14）+ 32 条 output 逐字节赋值
//   （每字节重读 ctx->m_state[i]）。原局部 k_padPrefix 栈数组 + for 循环
//   形（138 条）改为展开形。P167 CLEAR（2/2，修前修后一致）。
static unsigned char sha256_padding[0x40] = { 0x80 };

void sha256_finish(sha256_context* ctx, unsigned char output[0x20])
{
    unsigned char bits[8];
    unsigned long last;
    unsigned long padn;
    unsigned long hi;
    unsigned long lo;

    // bitlen 高/低 32 位（ORIG：hi=total0>>29|total1<<3; lo=total0<<3）
    hi = (ctx->m_total[0] >> 29) | (ctx->m_total[1] << 3);
    lo = ctx->m_total[0] << 3;

    bits[0] = static_cast<unsigned char>(hi >> 24);
    bits[1] = static_cast<unsigned char>(hi >> 16);
    bits[2] = static_cast<unsigned char>(hi >> 8);
    bits[3] = static_cast<unsigned char>(hi);
    bits[4] = static_cast<unsigned char>(lo >> 24);
    bits[5] = static_cast<unsigned char>(lo >> 16);
    bits[6] = static_cast<unsigned char>(lo >> 8);
    bits[7] = static_cast<unsigned char>(lo);

    last = ctx->m_total[0] & 0x3f;
    padn = (last < 0x38) ? (0x38 - last) : (0x78 - last);

    sha256_update(ctx, sha256_padding, padn);
    sha256_update(ctx, bits, 8);

    output[0] = static_cast<unsigned char>(ctx->m_state[0] >> 24);
    output[1] = static_cast<unsigned char>(ctx->m_state[0] >> 16);
    output[2] = static_cast<unsigned char>(ctx->m_state[0] >> 8);
    output[3] = static_cast<unsigned char>(ctx->m_state[0]);
    output[4] = static_cast<unsigned char>(ctx->m_state[1] >> 24);
    output[5] = static_cast<unsigned char>(ctx->m_state[1] >> 16);
    output[6] = static_cast<unsigned char>(ctx->m_state[1] >> 8);
    output[7] = static_cast<unsigned char>(ctx->m_state[1]);
    output[8] = static_cast<unsigned char>(ctx->m_state[2] >> 24);
    output[9] = static_cast<unsigned char>(ctx->m_state[2] >> 16);
    output[10] = static_cast<unsigned char>(ctx->m_state[2] >> 8);
    output[11] = static_cast<unsigned char>(ctx->m_state[2]);
    output[12] = static_cast<unsigned char>(ctx->m_state[3] >> 24);
    output[13] = static_cast<unsigned char>(ctx->m_state[3] >> 16);
    output[14] = static_cast<unsigned char>(ctx->m_state[3] >> 8);
    output[15] = static_cast<unsigned char>(ctx->m_state[3]);
    output[16] = static_cast<unsigned char>(ctx->m_state[4] >> 24);
    output[17] = static_cast<unsigned char>(ctx->m_state[4] >> 16);
    output[18] = static_cast<unsigned char>(ctx->m_state[4] >> 8);
    output[19] = static_cast<unsigned char>(ctx->m_state[4]);
    output[20] = static_cast<unsigned char>(ctx->m_state[5] >> 24);
    output[21] = static_cast<unsigned char>(ctx->m_state[5] >> 16);
    output[22] = static_cast<unsigned char>(ctx->m_state[5] >> 8);
    output[23] = static_cast<unsigned char>(ctx->m_state[5]);
    output[24] = static_cast<unsigned char>(ctx->m_state[6] >> 24);
    output[25] = static_cast<unsigned char>(ctx->m_state[6] >> 16);
    output[26] = static_cast<unsigned char>(ctx->m_state[6] >> 8);
    output[27] = static_cast<unsigned char>(ctx->m_state[6]);
    output[28] = static_cast<unsigned char>(ctx->m_state[7] >> 24);
    output[29] = static_cast<unsigned char>(ctx->m_state[7] >> 16);
    output[30] = static_cast<unsigned char>(ctx->m_state[7] >> 8);
    output[31] = static_cast<unsigned char>(ctx->m_state[7]);
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
