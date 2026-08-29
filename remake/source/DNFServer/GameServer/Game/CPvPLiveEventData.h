#ifndef DNF_GAME_CPVPLIVEEVENTDATA_H_
#define DNF_GAME_CPVPLIVEEVENTDATA_H_

// df_game_r 还原 —— CPvPLiveEventData。
// ORIG：charac_expand::CData 派生，+0x08 播放计数 int。
#include "CBoosterGage.h"

class CPvPLiveEventData : public charac_expand::CData
{
public:
    CPvPLiveEventData();   // ORIG T 0x08269054
    virtual void ResetDailyMidnight();
    virtual void _reset();
    virtual bool loadData(class CUser* user, char* buffer);
    virtual void getData(char* pData) const;   // ORIG T 0x08269206（空序列化）
    virtual bool _saveData(class CUser* user);
    void IncreasePlayCount(class CUser* user);

    int m_playCount;      // +0x08
    char m_pad0c[0x34];   // 补足 ORIG 对象 0x40 视图
};

#endif
