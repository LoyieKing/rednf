// df_game_r 还原 —— Inter_MonitorGuildPointDel 静态成员定义（ORIG W 0x82a5a4c /
//   W 0x084eb100 / B 0x943dd88）。类声明唯一声明点 InterDispatcher.h（§4.4 合并）。
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

// ORIG W 0x084eb100：movl $0,(%esp); call time@plt; mov %eax,0x943dd88
void Inter_MonitorGuildPointDel::setLastDeleteTime()
{
    g_lastDeleteTime = time(0);
}
