#ifndef DNF_GAME_CPVPLIVEEVENTDATA_H_
#define DNF_GAME_CPVPLIVEEVENTDATA_H_

// df_game_r 还原 —— CPvPLiveEventData（G2-4 批次，PvP 直播活动计数）。
// 方法签名对照 ORIG（_ZN17CPvPLiveEventData* 族）。
class CPvPLiveEventData
{
public:
    CPvPLiveEventData() {}
    ~CPvPLiveEventData() {}

    void IncreasePlayCount(class CUser* user);
    void ResetDailyMidnight();
    void _reset();
    void loadData(class CUser* user, char* buffer);
    void _saveData(class CUser* user);

    char m_pad[0x40];
};

#endif  // DNF_GAME_CPVPLIVEEVENTDATA_H_