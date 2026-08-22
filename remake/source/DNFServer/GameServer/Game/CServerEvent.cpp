// df_game_r 还原 —— CServerEvent::GetExpRate（G2-4 批次，服务端事件状态）。
// 仅定义 GetExpRate（ORIG _ZN12CServerEvent10GetExpRateEv）。
// 其余方法（AddExpRate/SubExpRate/IsEventing/TurnOnEvent/TurnOffEvent/
// GetEventInterval）仅声明不定义，避免与其它 TU 冲突。

#include "CServerEvent.h"

int CServerEvent::GetExpRate()
{
    return 0;
}