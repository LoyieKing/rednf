// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ShaSupport/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
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
#include "ShaSupport/sha256_starts.inc"
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

#include "ShaSupport/sha256_process.inc"
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
#include "ShaSupport/sha256_update.inc"
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

#include "ShaSupport/sha256_finish.inc"
}

// ---------------------------------------------------------------------------
// StrCpyMaxPath(char* dest, const char* src) T @0x0808a5dd
// memset(dest,0,0x104); strncpy(dest,src,0x103)（PLT 调用逐参数转写）。
// ---------------------------------------------------------------------------
#include "ShaSupport/StrCpyMaxPath.inc"
}
