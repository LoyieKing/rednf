#ifndef DNF_GAME_MD5_H_
#define DNF_GAME_MD5_H_

struct md5_context
{
    char m_pad[224];   // 权威定义归口本头；CInventory.h 同名区已删除重复。
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

#endif  // DNF_GAME_MD5_H_
