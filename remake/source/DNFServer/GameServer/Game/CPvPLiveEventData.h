#ifndef DNF_GAME_CPVPLIVEEVENTDATA_H_
#define DNF_GAME_CPVPLIVEEVENTDATA_H_

// df_game_r 还原 —— CPvPLiveEventData（G2-4 批次，PvP 直播活动计数）。
// ORIG：charac_expand::CData 派生（GetCharacExpandData(3) 强转目标），
// +0x08 为播放计数 int（IncreasePlayCount 0x826921c 实证）。
#include "CBoosterGage.h"   // charac_expand::CData

class CPvPLiveEventData : public charac_expand::CData
{
public:
    void IncreasePlayCount(class CUser* user);   // ORIG 0x826921c
    void ResetDailyMidnight();
    void _reset();

    int m_playCount;      // +0x08（ORIG IncreasePlayCount 计数）
    bool loadData(class CUser* user, char* buffer);   // 覆盖 CData::loadData
    bool _saveData(class CUser* user);                // 覆盖 CData::_saveData
    char m_pad0c[0x34];   // 补足 ORIG 对象 0x40 视图（后续批次按报告细化）
};

#endif  // DNF_GAME_CPVPLIVEEVENTDATA_H_
