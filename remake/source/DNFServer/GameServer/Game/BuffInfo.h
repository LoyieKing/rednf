// df_game_r Game/ BuffInfo（批3：从 GameStubs.cpp 迁移）。
// O0 C++98 布局：char m_pad[0x10] 缓冲。
#ifndef DNF_BUFFINFO_H
#define DNF_BUFFINFO_H

class BuffInfo
{
public:
    BuffInfo();
    void reset();

    char m_pad[0x10];  // +0x00
};

#endif  // DNF_BUFFINFO_H