// df_game_r 还原 —— Inter_MonitorGuildPointDel::getLastDeleteTime（ORIG W 0x82a5a4c）
//   objdump（082a5a4c）: mov 0x943dd88,%eax; ret  —— 直接返回静态 g_lastDeleteTime
//   （ORIG B _ZN26Inter_MonitorGuildPointDel16g_lastDeleteTimeE @ 0x943dd88）。
//   ORIG mangled _ZN26Inter_MonitorGuildPointDel17getLastDeleteTimeEv。
// 类声明见 Inter_MonitorGuildPointDel.h（唯一声明点）。

#include "Inter_MonitorGuildPointDel.h"

int Inter_MonitorGuildPointDel::g_lastDeleteTime;

int Inter_MonitorGuildPointDel::getLastDeleteTime()
{
    return g_lastDeleteTime;
}
