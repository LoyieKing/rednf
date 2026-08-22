#ifndef DNF_GAME_INTER_MONITOR_GUILD_POINT_DEL_H_
#define DNF_GAME_INTER_MONITOR_GUILD_POINT_DEL_H_

// df_game_r 还原 —— Inter_MonitorGuildPointDel::getLastDeleteTime（ORIG W 0x82a5a4c）
//   objdump（082a5a4c）: mov 0x943dd88,%eax; ret  —— 直接返回静态 g_lastDeleteTime
//   （ORIG B _ZN26Inter_MonitorGuildPointDel16g_lastDeleteTimeE @ 0x943dd88）。
//   ORIG mangled _ZN26Inter_MonitorGuildPointDel17getLastDeleteTimeEv。
// 原类定义位于 Inter_MonitorGuildPointDel.cpp 内部；因 CGameManager.cpp 需真实
// 调用 getLastDeleteTime（原 asm 桥），拆到本头作为唯一声明点。

class Inter_MonitorGuildPointDel
{
public:
    static int getLastDeleteTime();  // ORIG 汇编无 this 压栈（直接返回静态成员）
    static int g_lastDeleteTime;
};

#endif  // DNF_GAME_INTER_MONITOR_GUILD_POINT_DEL_H_
