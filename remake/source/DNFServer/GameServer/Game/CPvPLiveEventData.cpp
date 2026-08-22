// df_game_r 还原 —— CPvPLiveEventData::IncreasePlayCount（G2-4 批次，
// PvP 直播活动计数）。
// 仅定义 IncreasePlayCount（ORIG _ZN17CPvPLiveEventData17IncreasePlayCountEP5CUser）。
// 其余方法（ResetDailyMidnight/_reset/loadData/_saveData）仅声明不定义。

#include "CPvPLiveEventData.h"

#include "CUser.h"

void CPvPLiveEventData::IncreasePlayCount(class CUser* user)
{
    (void)user;
}