// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 md5/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ MD5 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 算法正确即可，不逐指令对齐。
//
// [Md5RestLane·2026-09-11 二批] 按 ORIG 结构补齐 `_ZL11md5_processP11md5_contextPh`
// （原我方把压缩宏内联进 md5_update）：update/finish/process 三件现与 ORIG 同构
// —— total 为**字节计数**（total[0] += len；finish 才 <<3 出 bit 长度）、
// update 每 64B 调 md5_process、finish 用 rodata `md5_padding` 并逐字节展开写 out。
// 同时删除我方发明符号 `_ZL8md5_rotlji`（ORIG 无；旋转改由 rotate 表达式内联）。
#include "md5.h"
#include <cstring>
#include <cstdio>

#include "md5/md5_starts.inc"
{
    // P183：不缓存视图指针（ORIG 0x080b2ae0 每次 `mov 0x8(%ebp),%eax` 重载 ctx）。
    ((md5_ctx*)ctx)->total[0] = 0;
    ((md5_ctx*)ctx)->total[1] = 0;
    ((md5_ctx*)ctx)->state[0] = 0x67452301;
    ((md5_ctx*)ctx)->state[1] = 0xefcdab89;
    ((md5_ctx*)ctx)->state[2] = 0x98badcfe;
    ((md5_ctx*)ctx)->state[3] = 0x10325476;
}

// ============================================================================
// md5_process —— ORIG t _ZL11md5_processP11md5_contextPh @0x080b2b20，size 0xf6d
// （TU-local static；唯一调用者是 md5_update：0x080b3b24/0x080b3b4b 两处）。
//
// 反汇编要点（objdump -d ORIG 0x080b2b20..0x080b3a8d）：
//   ① 序言 sub $0x50：x[16] 局部占 -0x50..-0x14，逐 dword 由 data 4 字节小端装配
//      （`movzbl (%eax)` / `movzbl 1(%edx); shl $0x8; or` / `shl $0x10` / `shl $0x18`）
//      ⇒ `x[i] = data[4i] | data[4i+1]<<8 | data[4i+2]<<16 | data[4i+3]<<24;`
//   ② a/b/c/d 四个具名 state 局部 @-0x10/-0xc/-0x8/-0x4：从 ctx->state[0..3] 载入，
//      64 步后逐条累加回 ctx->state（`mov -0x10(%ebp),%eax; add %eax,0x8(%edx)` 形）。
//   ③ 轮函数 F/G/H/I + 64 步（常量 0xd76aa478…0xeb86d391，位移 7/12/17/22…）；
//      旋转是单条 `ror $0xNN,%eax`（ORIG 无 helper 调用）⇒ 重建用 rotate 表达式，
//      不再经 `_ZL8md5_rotlji`（我方发明符号，已删）。
//   ④ 每步三段式（与 ORIG 同）：`a += F(b,c,d) + x[k] + t; a = ROTL(a,s); a += b;`
//      且 x[k] 以**局部数组下标**直读（ORIG `mov -0x44(%ebp),%eax`）。
//   帧：sub $0x50（无 callee-saved push）。
// ============================================================================
#include "md5/md5_process.inc"
{
    unsigned int x[16];
#define MD5_LOAD_X(i) \
    x[i] = (unsigned int)data[(i) * 4] \
         | ((unsigned int)data[(i) * 4 + 1] << 8) \
         | ((unsigned int)data[(i) * 4 + 2] << 16) \
         | ((unsigned int)data[(i) * 4 + 3] << 24);
    MD5_LOAD_X(0)
    MD5_LOAD_X(1)
    MD5_LOAD_X(2)
    MD5_LOAD_X(3)
    MD5_LOAD_X(4)
    MD5_LOAD_X(5)
    MD5_LOAD_X(6)
    MD5_LOAD_X(7)
    MD5_LOAD_X(8)
    MD5_LOAD_X(9)
    MD5_LOAD_X(10)
    MD5_LOAD_X(11)
    MD5_LOAD_X(12)
    MD5_LOAD_X(13)
    MD5_LOAD_X(14)
    MD5_LOAD_X(15)
#undef MD5_LOAD_X
    unsigned int a = ((md5_ctx*)ctx)->state[0];
    unsigned int b = ((md5_ctx*)ctx)->state[1];
    unsigned int cc = ((md5_ctx*)ctx)->state[2];
    unsigned int d = ((md5_ctx*)ctx)->state[3];
#define MD5_F(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define MD5_G(x, y, z) (((x) & (z)) | ((y) & ~(z)))
#define MD5_H(x, y, z) ((x) ^ (y) ^ (z))
#define MD5_I(x, y, z) ((y) ^ ((x) | ~(z)))
#define MD5_STEP(f, a, b, c, d, x, t, s) \
    (a) += f((b), (c), (d)) + (x) + (t); \
    (a) = (((a) << (s)) | ((a) >> (32 - (s)))) + (b);
    MD5_STEP(MD5_F, a, b, cc, d, x[0], 0xd76aa478, 7);
    MD5_STEP(MD5_F, d, a, b, cc, x[1], 0xe8c7b756, 12);
    MD5_STEP(MD5_F, cc, d, a, b, x[2], 0x242070db, 17);
    MD5_STEP(MD5_F, b, cc, d, a, x[3], 0xc1bdceee, 22);
    MD5_STEP(MD5_F, a, b, cc, d, x[4], 0xf57c0faf, 7);
    MD5_STEP(MD5_F, d, a, b, cc, x[5], 0x4787c62a, 12);
    MD5_STEP(MD5_F, cc, d, a, b, x[6], 0xa8304613, 17);
    MD5_STEP(MD5_F, b, cc, d, a, x[7], 0xfd469501, 22);
    MD5_STEP(MD5_F, a, b, cc, d, x[8], 0x698098d8, 7);
    MD5_STEP(MD5_F, d, a, b, cc, x[9], 0x8b44f7af, 12);
    MD5_STEP(MD5_F, cc, d, a, b, x[10], 0xffff5bb1, 17);
    MD5_STEP(MD5_F, b, cc, d, a, x[11], 0x895cd7be, 22);
    MD5_STEP(MD5_F, a, b, cc, d, x[12], 0x6b901122, 7);
    MD5_STEP(MD5_F, d, a, b, cc, x[13], 0xfd987193, 12);
    MD5_STEP(MD5_F, cc, d, a, b, x[14], 0xa679438e, 17);
    MD5_STEP(MD5_F, b, cc, d, a, x[15], 0x49b40821, 22);
    MD5_STEP(MD5_G, a, b, cc, d, x[1], 0xf61e2562, 5);
    MD5_STEP(MD5_G, d, a, b, cc, x[6], 0xc040b340, 9);
    MD5_STEP(MD5_G, cc, d, a, b, x[11], 0x265e5a51, 14);
    MD5_STEP(MD5_G, b, cc, d, a, x[0], 0xe9b6c7aa, 20);
    MD5_STEP(MD5_G, a, b, cc, d, x[5], 0xd62f105d, 5);
    MD5_STEP(MD5_G, d, a, b, cc, x[10], 0x02441453, 9);
    MD5_STEP(MD5_G, cc, d, a, b, x[15], 0xd8a1e681, 14);
    MD5_STEP(MD5_G, b, cc, d, a, x[4], 0xe7d3fbc8, 20);
    MD5_STEP(MD5_G, a, b, cc, d, x[9], 0x21e1cde6, 5);
    MD5_STEP(MD5_G, d, a, b, cc, x[14], 0xc33707d6, 9);
    MD5_STEP(MD5_G, cc, d, a, b, x[3], 0xf4d50d87, 14);
    MD5_STEP(MD5_G, b, cc, d, a, x[8], 0x455a14ed, 20);
    MD5_STEP(MD5_G, a, b, cc, d, x[13], 0xa9e3e905, 5);
    MD5_STEP(MD5_G, d, a, b, cc, x[2], 0xfcefa3f8, 9);
    MD5_STEP(MD5_G, cc, d, a, b, x[7], 0x676f02d9, 14);
    MD5_STEP(MD5_G, b, cc, d, a, x[12], 0x8d2a4c8a, 20);
    MD5_STEP(MD5_H, a, b, cc, d, x[5], 0xfffa3942, 4);
    MD5_STEP(MD5_H, d, a, b, cc, x[8], 0x8771f681, 11);
    MD5_STEP(MD5_H, cc, d, a, b, x[11], 0x6d9d6122, 16);
    MD5_STEP(MD5_H, b, cc, d, a, x[14], 0xfde5380c, 23);
    MD5_STEP(MD5_H, a, b, cc, d, x[1], 0xa4beea44, 4);
    MD5_STEP(MD5_H, d, a, b, cc, x[4], 0x4bdecfa9, 11);
    MD5_STEP(MD5_H, cc, d, a, b, x[7], 0xf6bb4b60, 16);
    MD5_STEP(MD5_H, b, cc, d, a, x[10], 0xbebfbc70, 23);
    MD5_STEP(MD5_H, a, b, cc, d, x[13], 0x289b7ec6, 4);
    MD5_STEP(MD5_H, d, a, b, cc, x[0], 0xeaa127fa, 11);
    MD5_STEP(MD5_H, cc, d, a, b, x[3], 0xd4ef3085, 16);
    MD5_STEP(MD5_H, b, cc, d, a, x[6], 0x04881d05, 23);
    MD5_STEP(MD5_H, a, b, cc, d, x[9], 0xd9d4d039, 4);
    MD5_STEP(MD5_H, d, a, b, cc, x[12], 0xe6db99e5, 11);
    MD5_STEP(MD5_H, cc, d, a, b, x[15], 0x1fa27cf8, 16);
    MD5_STEP(MD5_H, b, cc, d, a, x[2], 0xc4ac5665, 23);
    MD5_STEP(MD5_I, a, b, cc, d, x[0], 0xf4292244, 6);
    MD5_STEP(MD5_I, d, a, b, cc, x[7], 0x432aff97, 10);
    MD5_STEP(MD5_I, cc, d, a, b, x[14], 0xab9423a7, 15);
    MD5_STEP(MD5_I, b, cc, d, a, x[5], 0xfc93a039, 21);
    MD5_STEP(MD5_I, a, b, cc, d, x[12], 0x655b59c3, 6);
    MD5_STEP(MD5_I, d, a, b, cc, x[3], 0x8f0ccc92, 10);
    MD5_STEP(MD5_I, cc, d, a, b, x[10], 0xffeff47d, 15);
    MD5_STEP(MD5_I, b, cc, d, a, x[1], 0x85845dd1, 21);
    MD5_STEP(MD5_I, a, b, cc, d, x[8], 0x6fa87e4f, 6);
    MD5_STEP(MD5_I, d, a, b, cc, x[15], 0xfe2ce6e0, 10);
    MD5_STEP(MD5_I, cc, d, a, b, x[6], 0xa3014314, 15);
    MD5_STEP(MD5_I, b, cc, d, a, x[13], 0x4e0811a1, 21);
    MD5_STEP(MD5_I, a, b, cc, d, x[4], 0xf7537e82, 6);
    MD5_STEP(MD5_I, d, a, b, cc, x[11], 0xbd3af235, 10);
    MD5_STEP(MD5_I, cc, d, a, b, x[2], 0x2ad7d2bb, 15);
    MD5_STEP(MD5_I, b, cc, d, a, x[9], 0xeb86d391, 21);
#undef MD5_F
#undef MD5_G
#undef MD5_H
#undef MD5_I
#undef MD5_STEP
    ((md5_ctx*)ctx)->state[0] += a;
    ((md5_ctx*)ctx)->state[1] += b;
    ((md5_ctx*)ctx)->state[2] += cc;
    ((md5_ctx*)ctx)->state[3] += d;
}

// ============================================================================
// md5_update —— ORIG T 0x080b3a8d，size 0x100（**字节计数**版，与 md5_process 配套）。
//
// 反汇编要点（objdump -d ORIG 0x080b3a8d..0x080b3b8d）：
//   ① `cmpl $0x0,0x10(%ebp); jle` ⇒ `if (len <= 0) return;`
//   ② used@-0xc = `ctx->total[0] & 0x3f`；fill@-0x10 = `0x40 - used`
//      （total[0] 为**字节**计数，故不 >>3）。
//   ③ `ctx->total[0] += len`；`cmp %eax,%edx; jae` ⇒ `if (total[0] < len) ++total[1]`（无符号）。
//   ④ `cmpl $0x0,-0xc; je tail` + `cmp -0x10(%ebp),%eax; jl tail` ⇒
//      `if (used != 0 && len >= (int)fill)` 内：memcpy(buffer+used, data, fill) →
//      md5_process(ctx, buffer)（0x080b3b24）→ `data += fill; len -= fill; used = 0;`。
//   ⑤ while 前置 `jmp 条件` + 尾 `cmpl $0x3f,0x10(%ebp); setg %al; test; jne` ⇒
//      `while (len > 0x3f) { md5_process(ctx, data); data += 0x40; len -= 0x40; }`（普通 while，
//      调用点 0x080b3b4b 直接传 data，不经 buffer）。
//   ⑥ 尾 `cmpl $0x0,0x10(%ebp); jle` 守卫 + memcpy(buffer+used, data, len) ⇒
//      `if (len > 0) memcpy(ctx->buffer + used, data, len);`
//   帧：sub $0x28；used@-0xc、fill@-0x10（无 callee-saved push、无指针局部）。
// ============================================================================
#include "md5/md5_update.inc"
{
    if (len <= 0)
        return;
    // 槽序：ORIG used@-0xc / fill@-0x10（4.4 同尺寸标量区按声明序「先声明更深」，
    // 故 fill 先声明、used 后声明；执行序仍是 used 赋值在前）。
    int fill;
    int used = (int)(((md5_ctx*)ctx)->total[0] & 0x3f);
    fill = 0x40 - used;
    ((md5_ctx*)ctx)->total[0] += (unsigned int)len;
    if (((md5_ctx*)ctx)->total[0] < (unsigned int)len)
        ++((md5_ctx*)ctx)->total[1];
    if (used != 0 && len >= fill)
    {
        memcpy(((md5_ctx*)ctx)->buffer + used, data, fill);
        md5_process(ctx, ((md5_ctx*)ctx)->buffer);
        data += fill;
        len -= fill;
        used = 0;
    }
    while (len > 0x3f)
    {
        md5_process(ctx, data);
        data += 0x40;
        len -= 0x40;
    }
    if (len > 0)
        memcpy(((md5_ctx*)ctx)->buffer + used, data, len);
}

// ============================================================================
// md5_padding —— ORIG r @0x08b1f0e0，size 0x40（.rodata 逐字节 80 00 … 00 = 0x80 + 63×0）。
// md5_finish 0x080b3c25 以立即数 $0x8b1f0e0 作为 md5_update 的第二实参。
// ============================================================================
// 注：C++ namespace-scope `const` 隐式内部链接 ⇒ 符号名 = 裸名 + local(r)；
// 加 `static` 会变 `_ZL11md5_padding`（与 ORIG 名不符），故不写 static。
const unsigned char md5_padding[0x40] = {0x80};

// ============================================================================
// md5_finish —— ORIG T 0x080b3b8d，size 0x1ca。
//
// 反汇编要点（objdump -d ORIG 0x080b3b8d..0x080b3d57）：
//   ① bit 长度高低 32 位（total 为字节计数）：
//      hi@-0x10 = `(total[0] >> 29) | (total[1] << 3)`；lo@-0xc = `total[0] << 3`。
//   ② lenbytes[8] @-0x20..-0x19：lo 4 字节在下、hi 4 字节在上（逐字节 `shr $8/$0x10/$0x18`）。
//   ③ used@-0x18 = `total[0] & 0x3f`；padlen@-0x14 三目
//      `cmpl $0x37,-0x18; ja else` ⇒ `used <= 0x37 ? 0x38 - used : 0x78 - used`（无符号）。
//   ④ `md5_update(ctx, md5_padding, padlen)`（0x080b3c33，立即数 $0x8b1f0e0）
//      → `md5_update(ctx, lenbytes, 8)`。
//   ⑤ 摘要按字节展开直写 out（16 条 `mov state[i]` + `shr`；每次重载 out，无循环）。
//   帧：sub $0x38；hi/lo/lenbytes/used/padlen 五个局部。
// ============================================================================
#include "md5/md5_finish.inc"
{
    // 全头部声明 + 声明序 = ORIG 槽深→浅（P212/P339；4.4 对函数头标量按
    // 「先声明更深」分配）：lenbytes(-0x20) → used(-0x18) → padlen(-0x14)
    // → hi(-0x10) → lo(-0xc)。执行序仍按 ORIG 反汇编（hi、lo、lenbytes、used、padlen）。
    unsigned char lenbytes[8];
    unsigned int used;
    unsigned int padlen;
    unsigned int hi;
    unsigned int lo;
    hi = (((md5_ctx*)ctx)->total[0] >> 29) | (((md5_ctx*)ctx)->total[1] << 3);
    lo = ((md5_ctx*)ctx)->total[0] << 3;
    lenbytes[0] = (unsigned char)lo;
    lenbytes[1] = (unsigned char)(lo >> 8);
    lenbytes[2] = (unsigned char)(lo >> 16);
    lenbytes[3] = (unsigned char)(lo >> 24);
    lenbytes[4] = (unsigned char)hi;
    lenbytes[5] = (unsigned char)(hi >> 8);
    lenbytes[6] = (unsigned char)(hi >> 16);
    lenbytes[7] = (unsigned char)(hi >> 24);
    used = ((md5_ctx*)ctx)->total[0] & 0x3f;
    padlen = used <= 0x37 ? 0x38 - used : 0x78 - used;
    md5_update(ctx, (unsigned char*)md5_padding, (int)padlen);
    md5_update(ctx, lenbytes, 8);
    out[0] = (unsigned char)(((md5_ctx*)ctx)->state[0]);
    out[1] = (unsigned char)(((md5_ctx*)ctx)->state[0] >> 8);
    out[2] = (unsigned char)(((md5_ctx*)ctx)->state[0] >> 16);
    out[3] = (unsigned char)(((md5_ctx*)ctx)->state[0] >> 24);
    out[4] = (unsigned char)(((md5_ctx*)ctx)->state[1]);
    out[5] = (unsigned char)(((md5_ctx*)ctx)->state[1] >> 8);
    out[6] = (unsigned char)(((md5_ctx*)ctx)->state[1] >> 16);
    out[7] = (unsigned char)(((md5_ctx*)ctx)->state[1] >> 24);
    out[8] = (unsigned char)(((md5_ctx*)ctx)->state[2]);
    out[9] = (unsigned char)(((md5_ctx*)ctx)->state[2] >> 8);
    out[10] = (unsigned char)(((md5_ctx*)ctx)->state[2] >> 16);
    out[11] = (unsigned char)(((md5_ctx*)ctx)->state[2] >> 24);
    out[12] = (unsigned char)(((md5_ctx*)ctx)->state[3]);
    out[13] = (unsigned char)(((md5_ctx*)ctx)->state[3] >> 8);
    out[14] = (unsigned char)(((md5_ctx*)ctx)->state[3] >> 16);
    out[15] = (unsigned char)(((md5_ctx*)ctx)->state[3] >> 24);
}

// 一步式封装（ORIG T _Z3md5PhiS_ @0x080b3d57：starts→update(data,len)→finish→
// memset(ctx,0,0xd8)；GameSecurity::_NS_MakeHash_ 0x0808c8f0 唯一调用点）。
#include "md5/md5.inc"
{
    md5_context ctx;
    md5_starts(&ctx);
    md5_update(&ctx, data, len);
    md5_finish(&ctx, digest);
    memset(&ctx, 0, 0xd8);
}

// ============================================================================
// md5_merging —— ORIG T 0x080b3dbf，size 0xa2（同 TU：md5_finish 0x080b3b8d /
// md5 0x080b3d57 / 本函数 0x080b3dbf / md5_file 0x080b3e61，地址连续）。
//
// 反汇编要点（objdump -d ORIG 0x080b3dbf..0x080b3e61）：
//   ① srcLen clamp：`mov 0xc(%ebp),%eax; cmp $0x400,%eax; jle +2; mov $0x400,%eax;
//      mov %eax,-0x14(%ebp)` ⇒ 具名 int 局部（P0：有槽往返），三目取值。
//   ② memcpy(tmp,src,len)：size/src/dst 依右→左压栈，dst 为立即数 $0x93fa240
//      ⇒ 函数局部 static 缓冲（ORIG 符号 _ZZ11md5_mergingPKciS0_iPhE3tmp @0x093fa240，size 0x400）。
//   ③ 循环：i@-0x10 / j@-0xc 两个具名 int（movl $0,-0x10 / movl $0,-0xc），
//      `jmp` 前置条件 + 尾 `cmp 0x14(%ebp),%eax; setl %al; test %al,%al; jne` ⇒ while(j<dstLen)。
//   ④ `if (i >= len) i = 0;`：`cmp -0x14(%ebp),%eax; jl skip; movl $0,-0x10`。
//   ⑤ `tmp[i] = tmp[i] ^ dst[j]`：i 读两次（LHS 索引 eax / RHS 索引 edx），
//      tmp[i] 与 dst[j] 均为 movzbl 零扩展 ⇒ 两侧都按 unsigned char 语义参与 xor。
//   ⑥ `++i; ++j;`：`addl $0x1,-0x10` / `addl $0x1,-0xc`。
//   ⑦ 尾 `md5(tmp,len,out)`（out→0x8(%esp)、len→0x4(%esp)、tmp→(%esp)），
//      随后直接 `leave; ret`（无 %eax 置位）⇒ 返回 void。
//   帧：push ebp/mov esp,ebp/sub $0x28（无 callee-saved push）。
//   注：ORIG 全库无该符号的调用点（objdump 全 .text `call 0x80b3dbf` 命中 0 处），
//       系独立导出函数；本实现按语义+逐指令转写落位。
// ============================================================================
#include "md5/md5_merging.inc"
{
    static unsigned char tmp[0x400];
    // clamp 写作 if 形（非三目）：ORIG 4.1 无 cmov，为
    // `mov 0xc(%ebp),%eax; cmp $0x400,%eax; jle +2; mov $0x400,%eax; mov %eax,-0x14(%ebp)`；
    // 4.4 对三目生 MIN_EXPR cmovle（P230①），改 if 形即出 jle 跳转族。
    int len = srcLen;
    if (len > 0x400)
        len = 0x400;
    memcpy(tmp, src, len);
    int i = 0;
    int j = 0;
    while (j < dstLen)
    {
        if (i >= len)
            i = 0;
        tmp[i] = tmp[i] ^ (unsigned char)dst[j];
        ++i;
        ++j;
    }
    md5(tmp, len, out);
}

// ============================================================================
// md5_file —— ORIG T 0x080b3e61，size 0x101（同 TU：md5 0x080b3d57 / md5_merging
// 0x080b3dbf / 本函数 0x080b3e61 / md5_hmac_starts 0x080b3f62，地址连续 ⇒ 本聚合 TU）。
//
// 反汇编要点（objdump -d ORIG 0x080b3e61..0x080b3f62）：
//   ① fopen(filename, "rb")：mode 串立即数 $0x8b1f0a0（.rodata 逐字节 72 62 00 =
//      "rb"），实参装载序 mode→0x4(%esp)、filename→(%esp)；fp@-0x10。
//   ② `fp == 0`：`cmpl $0x0,-0x10; sete %al; test %al,%al; je` ⇒ `if (fp == 0)
//      return 1;`（常量 1 落尾块）。
//   ③ md5_starts(&ctx)：ctx 槽基 -0xe8（0xd8 字节：-0xe8..-0x10，紧贴上邻 fp/len）。
//   ④ 读循环：`fread(buf,1,0x400,fp)` → len@-0xc；尾测 `cmpl $0x0,-0xc; setne %al;
//      test %al,%al; jne <body>` ⇒ `while ((len = fread(buf,1,0x400,fp)) != 0)
//      md5_update(&ctx,buf,len);`（入口先 `jmp` 条件块 = while 前置形，P25）。
//   ⑤ md5_finish(&ctx,out) → memset(&ctx,0,0xd8)（长度立即数 0xd8）。
//   ⑥ `ferror(fp)!=0` ⇒ fclose+return 2；否则 fclose+return 0（两个独立调用点）。
//   帧：sub $0x4f8；buf[0x400]@-0x4e8、ctx@-0xe8、fp@-0x10、len@-0xc。
// ============================================================================
#include "md5/md5_file.inc"
{
    FILE* fp = fopen(filename, "rb");
    if (fp == 0)
        return 1;
    // md5_context 已按 ORIG 实尺寸 0xd8 修正（md5.h），直接用本体、不再 cast
    // （Main 新范式「接收者去 cast ⇒ 实参装载序翻回 ORIG 的实参先于 this」）。
    md5_context ctx;
    unsigned char buf[0x400];
    int len;
    md5_starts(&ctx);
    while ((len = (int)fread(buf, 1, 0x400, fp)) != 0)
    {
        md5_update(&ctx, buf, len);
    }
    md5_finish(&ctx, out);
    memset(&ctx, 0, 0xd8);
    if (ferror(fp) != 0)
    {
        fclose(fp);
        return 2;
    }
    fclose(fp);
    return 0;
}

// ============================================================================
// md5_hmac_starts —— ORIG T 0x080b3f62，size 0x115。
//
// 反汇编要点（objdump -d ORIG 0x080b3f62..0x080b4077）：
//   ① `cmpl $0x40,0x10(%ebp); jle` ⇒ `if (keylen > 0x40)`；体内
//      md5(key,keylen,sum)（sum@-0x1c，0x10 字节），随后两条直写形参槽：
//      `movl $0x10,0x10(%ebp)` / `lea -0x1c; mov %eax,0xc(%ebp)` ⇒ `keylen = 0x10;
//      key = sum;`（P322 参数对象语义：写的是形参槽，不是新局部）。
//   ② memset(ctx+0x58,0x36,0x40) / memset(ctx+0x98,0x5c,0x40)：ipad/opad 初值
//      （md5_hmac_ctx 视图 +0x58 / +0x98）。
//   ③ for 循环 i@-0xc（`movl $0x0,-0xc` + 入口 jmp 条件 + 尾 `cmp 0x10(%ebp),%eax;
//      setl %al; test %al; jne`，P2/P25）：体内两条独立读改写
//      `c->m_ipad[i] ^= key[i];` / `c->m_opad[i] ^= key[i];`（索引各读 3 次、
//      movzbl 两侧、`xor %edx,%ecx`）。
//   ④ md5_starts(ctx) → md5_update(ctx, ctx+0x58, 0x40) → memset(sum,0,0x10)。
//   帧：sub $0x38（无 callee-saved push）。
// ============================================================================
#include "md5/md5_hmac_starts.inc"
{
    unsigned char sum[16];
    if (keylen > 0x40)
    {
        md5(key, keylen, sum);
        keylen = 0x10;
        key = sum;
    }
    // P183：不缓存视图指针（`md5_hmac_ctx* c = (…)ctx;` 会让 4.4 多占一个指针
    // 槽并把后续每次访问改成槽回读；ORIG 每次就地 `mov 0x8(%ebp),%eax; add $0x58`）。
    memset(((md5_hmac_ctx*)ctx)->m_ipad, 0x36, 0x40);
    memset(((md5_hmac_ctx*)ctx)->m_opad, 0x5c, 0x40);
    int i;
    for (i = 0; i < keylen; ++i)
    {
        ((md5_hmac_ctx*)ctx)->m_ipad[i] ^= key[i];
        ((md5_hmac_ctx*)ctx)->m_opad[i] ^= key[i];
    }
    md5_starts(ctx);
    md5_update(ctx, ((md5_hmac_ctx*)ctx)->m_ipad, 0x40);
    memset(sum, 0, 0x10);
}

// ============================================================================
// md5_hmac_update —— ORIG T 0x080b4077，size 0x21（唯一语义 = 转发 md5_update）。
//
// 反汇编要点（objdump -d ORIG 0x080b4077..0x080b4098）：实参 len→0x8(%esp)、
// data→0x4(%esp)、ctx→(%esp)，`call md5_update`；`leave; ret` 不置 %eax ⇒ void。
// 帧：sub $0x18。
// ============================================================================
#include "md5/md5_hmac_update.inc"
{
    md5_update(ctx, data, len);
}

// ============================================================================
// md5_hmac_finish —— ORIG T 0x080b4098，size 0x8b。
//
// 反汇编要点（objdump -d ORIG 0x080b4098..0x080b4123）：
//   ① md5_finish(ctx, &tmp)（tmp@-0x18，0x10 字节）—— 取 inner 摘要；
//   ② md5_starts(ctx)；
//   ③ md5_update(ctx, ctx+0x98, 0x40) —— opad 块（md5_hmac_ctx +0x98）；
//   ④ md5_update(ctx, &tmp, 0x10)（长度立即数 0x10 落 0x8(%esp)）；
//   ⑤ md5_finish(ctx, out)（out = 0xc(%ebp)）；
//   ⑥ memset(&tmp, 0, 0x10)。
//   帧：sub $0x28。
// ============================================================================
#include "md5/md5_hmac_finish.inc"
{
    unsigned char tmp[16];
    md5_finish(ctx, tmp);
    md5_starts(ctx);
    md5_update(ctx, ((md5_hmac_ctx*)ctx)->m_opad, 0x40);
    md5_update(ctx, tmp, 0x10);
    md5_finish(ctx, out);
    memset(tmp, 0, 0x10);
}

// ============================================================================
// md5_hmac —— ORIG T 0x080b4123，size 0x76（一步式 HMAC-MD5）。
//
// 反汇编要点（objdump -d ORIG 0x080b4123..0x080b4199）：
//   ctx 局部 @-0xe0（0xd8 字节 ⇒ 与 md5_hmac_ctx 视图同尺寸）；
//   md5_hmac_starts(&ctx,key,keylen) → md5_hmac_update(&ctx,in,inlen) →
//   md5_hmac_finish(&ctx,out) → memset(&ctx,0,0xd8)；`leave; ret` 不置 %eax ⇒ void。
//   帧：sub $0xf8。
// ============================================================================
#include "md5/md5_hmac.inc"
{
    // 用 md5_context 本体（非 0xd8 承载的 cast 形）：cast 形虽使槽基 -0xe0 与 ORIG 相同，
    // 但 P277「cast 前置」会把 ctx 的 lea 提前到实参装载之前，助记符序列反 DIFF；
    // 本体形保持 NEAR（仅 lea 槽基 -0xe8 浅 8，N95 族）。
    md5_context ctx;
    md5_hmac_starts(&ctx, key, keylen);
    md5_hmac_update(&ctx, in, inlen);
    md5_hmac_finish(&ctx, out);
    memset(&ctx, 0, 0xd8);
}

// ============================================================================
// md5_self_test 用的 rodata 表（ORIG r 符号：md5_test_str @0x08b1f120 size 0x237、
// md5_test_sum @0x08b1f360 size 0x70；同族 md5_padding @0x08b1f0e0 见上文
// md5_finish 链，已按 ORIG 定义补齐）。
//
// 布局证据：md5_self_test 取串地址段 0x080b41c7 的索引步长 = i*81
// （shl$3 → add i → lea(,%eax,8) → add i*9 ⇒ i*9+i*72），且 0x237 = 7*81；
// 摘要表步长 = i*16（`shl $0x4`），0x70 = 7*16。最长向量为 80 字符 + NUL = 81。
// 表内容逐字节取自 ORIG .rodata（objdump -s -j .rodata 0x08b1f120..0x08b1f3d0）。
// ============================================================================
const char md5_test_str[7][0x51] =
{
    "",
    "a",
    "abc",
    "message digest",
    "abcdefghijklmnopqrstuvwxyz",
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
    "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
};

const unsigned char md5_test_sum[7][16] =
{
    { 0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04, 0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e },
    { 0x0c, 0xc1, 0x75, 0xb9, 0xc0, 0xf1, 0xb6, 0xa8, 0x31, 0xc3, 0x99, 0xe2, 0x69, 0x77, 0x26, 0x61 },
    { 0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0, 0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72 },
    { 0xf9, 0x6b, 0x69, 0x7d, 0x7c, 0xb7, 0x93, 0x8d, 0x52, 0x5a, 0x2f, 0x31, 0xaa, 0xf1, 0x61, 0xd0 },
    { 0xc3, 0xfc, 0xd3, 0xd7, 0x61, 0x92, 0xe4, 0x00, 0x7d, 0xfb, 0x49, 0x6c, 0xca, 0x67, 0xe1, 0x3b },
    { 0xd1, 0x74, 0xab, 0x98, 0xd2, 0x77, 0xd9, 0xf5, 0xa5, 0x61, 0x1c, 0x2c, 0x9f, 0x41, 0x9d, 0x9f },
    { 0x57, 0xed, 0xf4, 0xa2, 0x2b, 0xe3, 0xc9, 0x55, 0xac, 0x49, 0xda, 0x2e, 0x21, 0x07, 0xb6, 0x7a }
};

// ============================================================================
// md5_self_test —— ORIG T 0x080b4199，size 0xf8。
//
// 反汇编要点（objdump -d ORIG 0x080b4199..0x080b4291）：
//   ① i@-0xc：`movl $0x0,-0xc` + 入口 jmp 条件 + 尾 `addl $0x1,-0xc` /
//      `cmpl $0x6,-0xc; setle %al; test %al; jne` ⇒ `for (i = 0; i <= 6; ++i)`（P2：
//      setle 直证 `<=`）。
//   ② verbose 段：`cmpl $0x0,0x8(%ebp); je` ⇒ `if (verbose) printf("  MD5 test #%d: ",
//      i + 1);`（格式串立即数 $0x8b1f0a3，.rodata "  MD5 test #%d: "；第二实参先物化）。
//   ③ md5(md5_test_str[i], strlen(md5_test_str[i]), md)（md@-0x1c）：先算串地址
//      （步长 i*81）→ call strlen（结果 %ecx → 0x4(%esp)）→ 压 &md → call md5。
//   ④ memcmp(md, md5_test_sum[i], 0x10)（摘要表步长 i*16）；`test %eax,%eax; je`
//      ⇒ `if (memcmp(...) != 0)`；不等时 `if (verbose) puts("failed");`（串立即数
//      $0x8b1f0b4）→ return 1。
//   ⑤ 相等时 `if (verbose) puts("passed");`（串立即数 $0x8b1f0bb）。
//   ⑥ 循环后 `if (verbose) putchar('\n');`（0xa）→ return 0。
//   帧：sub $0x38。
// ============================================================================
#include "md5/md5_self_test.inc"
{
    int i;
    unsigned char md[16];
    for (i = 0; i <= 6; ++i)
    {
        if (verbose)
            printf("  MD5 test #%d: ", i + 1);
        md5((unsigned char*)md5_test_str[i], (int)strlen(md5_test_str[i]), md);
        if (memcmp(md, md5_test_sum[i], 0x10) != 0)
        {
            if (verbose)
                puts("failed");
            return 1;
        }
        if (verbose)
            puts("passed");
    }
    if (verbose)
        putchar('\n');
    return 0;
}
