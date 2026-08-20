#ifndef DNF_GAME_MD5_H_
#define DNF_GAME_MD5_H_

// df_game_r Game/ MD5 实现（独立 TU，从 GameStubs.cpp 拆出）。
// ORIG 符号：_Z10md5_startsP11md5_context / _Z9md5_updateP11md5_contextPhi / _Z10md5_finishP11md5_contextPh
// 用户规则：算法正确即可，不逐指令对齐。

struct md5_context
{
    char m_data[224];
};

struct md5_ctx
{
    unsigned int total[2];
    unsigned int state[4];
    unsigned char buffer[64];
};

void md5_starts(md5_context* ctx);
void md5_update(md5_context* ctx, unsigned char* data, int len);
void md5_finish(md5_context* ctx, unsigned char* out);

#endif  // DNF_GAME_MD5_H_
