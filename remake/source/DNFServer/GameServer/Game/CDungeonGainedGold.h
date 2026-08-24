#ifndef GAME_CDUNGEONGAINEDGOLD_H_
#define GAME_CDUNGEONGAINEDGOLD_H_

// ============================================================================
// df_game_r 还原 —— CDungeonGainedGold（副本内金币累计，ORIG 类尺寸 4）。
// 布局证据：docs/class_func_reports/_lt_global_gt_.md 对应段；
// CUser +0x8cf64 值成员。原定义在 CDungeonGainedGold.cpp 本地类块，
// 2026-08-24 提取为共享头（CUser.h 需完整类型；方法实现仍在原 TU）。
// ============================================================================

class CDungeonGainedGold
{
public:
    unsigned int incGainedGold(unsigned int amount);   // ORIG 0x850d4b6 族
    unsigned int getGainedGold();
    void setGainedGold(unsigned int value);
    void reset();

    unsigned int m_gainedGold;  // +0x00
};  // sizeof 4

#endif  // GAME_CDUNGEONGAINEDGOLD_H_
