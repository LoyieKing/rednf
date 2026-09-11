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
static void md5_process(md5_context* ctx, unsigned char* data)
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