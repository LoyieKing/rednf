// HdrCreate3: ORIG exported symbols, first .h declaration.
// Evidence: nm ORIG（nm_260 C1 0x088cc2fa / Clear 0x088cc1a0）+ 定义提取自
// Game/CharacterAIA2.cpp:196-213。sizeof 0xa4 = 41 float；Clear：+0x00..+0x24
// 十个 1.0f；循环 i=0..3 写 +0x28+4i；循环 k=0..16 写 +0x38+4k；尾部 +0x7c..+0xa0
// 十个 1.0f。Clear/ctor 内联体保持 cpp 内同体（接入时由宿主去重）。
#ifndef DNF_GAME_STRDCHARACTERSTATUSRATE_H_
#define DNF_GAME_STRDCHARACTERSTATUSRATE_H_

struct STRDCharacterStatusRate
{
    STRDCharacterStatusRate() { Clear(); }

    void Clear()
    {
        for (int i = 0; i < 10; ++i) m_head[i] = 1.0f;
        for (int i = 0; i <= 3; ++i) m_mid28[i] = 1.0f;
        for (int k = 0; k <= 16; ++k) m_mid38[k] = 1.0f;
        for (int i = 0; i < 10; ++i) m_tail[i] = 1.0f;
    }

    float m_head[10];   // +0x00..+0x24
    float m_mid28[4];   // +0x28..+0x34
    float m_mid38[17];  // +0x38..+0x78（k=16 → 0x78）
    float m_tail[10];   // +0x7c..+0xa0
};  // sizeof == 0xa4

typedef char assert_strd_rate_size[
    (sizeof(STRDCharacterStatusRate) == 0xa4) ? 1 : -1];

#endif  // DNF_GAME_STRDCHARACTERSTATUSRATE_H_
