// df_game_r Game/ BuffInfo（批3：从 GameStubs.cpp 迁移）。
// ORIG C1/reset 0x08256c8e：三字段写 1（movl $1 @+0/+4/+8），布局 0xc。
#ifndef DNF_BUFFINFO_H
#define DNF_BUFFINFO_H

class BuffInfo
{
public:
    BuffInfo();
    void reset();

    int m_exp;   // +0x00（ORIG reset 写 1）
    int m_gold;  // +0x04
    int m_drop;  // +0x08
};

#endif  // DNF_BUFFINFO_H
