#ifndef DNF_GAME_MD5_H_
#define DNF_GAME_MD5_H_

struct md5_context
{
    // ORIG 实尺寸 0xd8（216）——权威定义归口本头；CInventory.h 同名区已删除重复。
    // 证据：md5_file 的 ctx 槽 -0xe8..-0x10、md5_hmac 的 -0xe0..-0x8、
    // md5() 内 `memset(&ctx,0,0xd8)`、hmac ipad/opad @+0x58/+0x98（0x58+0x40+0x40）、
    // CNChecksumMd5 的 `new unsigned char[0xd8]`（= ORIG operator new(0xd8)）。
    // 原 224B(0xe0) 为历史占位，大 8 ⇒ 各消费 TU 局部 `md5_context ctx;` 槽基浅 8（N95 族）。
    char m_pad[0xd8];
};

// df_game_r Game/ MD5 实现（独立 TU，从 GameStubs.cpp 拆出）。
// ORIG 符号：_Z10md5_startsP11md5_context / _Z9md5_updateP11md5_contextPhi / _Z10md5_finishP11md5_contextPh
// 用户规则：算法正确即可，不逐指令对齐。

// md5_context 权威定义见 CInventory.h（char m_pad[224]，同 224B 布局）。
struct md5_ctx
{
    unsigned int total[2];
    unsigned int state[4];
    unsigned char buffer[64];
};

void md5_starts(md5_context* ctx);
void md5_update(md5_context* ctx, unsigned char* data, int len);
void md5_finish(md5_context* ctx, unsigned char* out);

// ORIG 符号：_Z3md5PhiS_ @0x080b3d57（一步式 md5，尾部 memset 清 ctx 后 leave/ret 不置 %eax ⇒ void）。
void md5(unsigned char* data, int len, unsigned char* digest);

// ORIG 符号：_Z11md5_mergingPKciS0_iPh @0x080b3dbf（dst 以回绕 XOR 混入 src 后取 MD5，
// 尾部 leave/ret 不置 %eax ⇒ void）。
void md5_merging(const char* src, int srcLen, const char* dst, int dstLen, unsigned char* out);

// ORIG 的 hmac 三件套把上下文按「md5 状态 0x58 + ipad 64B + opad 64B」使用
// （证据：md5_hmac_starts @0x080b3f62 对 ctx+0x58 / ctx+0x98 各 memset 0x40 并逐字节
//  XOR key；md5_hmac_finish @0x080b4098 从 ctx+0x98 取 opad），整体 0xd8 ——
// 与 ChecksumCrypto.cpp 的 `new unsigned char[0xd8]`（ORIG operator new(0xd8)）一致。
// 视图只提供具名访问，sizeof(md5_hmac_ctx) == 0xd8（推断：布局由上述偏移直证）。
// 另：md5_file 的本地上下文亦用本类型承载（ORIG 实尺寸 0xd8，而 md5_context
// 头内为 224B 占位），见 md5.cpp 的 md5_file 注释。
struct md5_hmac_ctx
{
    md5_ctx m_md5;               // +0x00（md5_starts/update/finish 的实参视图）
    unsigned char m_ipad[0x40];  // +0x58
    unsigned char m_opad[0x40];  // +0x98
};

// ORIG 符号：_Z8md5_filePcPh @0x080b3e61（fopen "rb" → 循环 fread 0x400 → md5_finish →
// memset(ctx,0,0xd8) → ferror/fclose；成功 0 / 打开失败 1 / 读错 2）。
int md5_file(char* filename, unsigned char* out);

// ORIG 符号：_Z15md5_hmac_startsP11md5_contextPhi @0x080b3f62（key>0x40 先经 md5 压成
// 16B；ipad=0x36 / opad=0x5c 各 XOR key 后 md5_starts + update(ipad,0x40)）。
void md5_hmac_starts(md5_context* ctx, unsigned char* key, int keylen);

// ORIG 符号：_Z15md5_hmac_updateP11md5_contextPhi @0x080b4077（转发 md5_update）。
void md5_hmac_update(md5_context* ctx, unsigned char* data, int len);

// ORIG 符号：_Z15md5_hmac_finishP11md5_contextPh @0x080b4098（inner 摘要 → md5_starts →
// update(opad,0x40) → update(inner,0x10) → 最终 finish；尾 memset(tmp,0,0x10)）。
void md5_hmac_finish(md5_context* ctx, unsigned char* out);

// ORIG 符号：_Z8md5_hmacPhiS_iS_ @0x080b4123（一步式 HMAC-MD5，尾 memset(ctx,0,0xd8)）。
void md5_hmac(unsigned char* key, int keylen, unsigned char* in, int inlen, unsigned char* out);

// ORIG 符号：_Z13md5_self_testi @0x080b4199（RFC1321 七组向量自检；0 通过 / 1 失败）。
int md5_self_test(int verbose);

#endif  // DNF_GAME_MD5_H_
