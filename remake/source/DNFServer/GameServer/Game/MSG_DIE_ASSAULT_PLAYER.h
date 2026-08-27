// df_game_r Game/ MSG_DIE_ASSAULT_PLAYER 权威布局（W6_AssaultMgr 批：唯一声明点）。
//
// 布局证据（ORIG 反汇编实证）：
//   1) Dispatcher_DieAssaultPlayer::dispatch_sig 0x08207208 栈帧：
//      get_int 写基址 +0x00、get_int 写 +0x04、连续 4 个 get_short 写
//      +0x08/+0x0a/+0x0c/+0x0e —— 读入 sizeof=0x10 后按引用传给
//      CAssaultMgr::OnDieUser(CUser*,int,MSG_DIE_ASSAULT_PLAYER&)。
//   2) pvp_assault::CAssaultPlace::DieUser(int,int,MSG&) 0x82ebda6：
//      *(int*)msg(+0x00)、*(uint*)(msg+0x04)/(uint)*(int*)msg 除法 hack 判定
//      （>100000 → CHackAnalyzer::addServerHackCnt 0x7e）；
//      cUserHistoryLog::DiePvP(…, msg+0 int, msg+4 int, msg+8/0xa/0xc/0xe short×4, …)。
// ORIG nm 无该类型 ctor/dtor/typeinfo 符号 → 纯 POD。
#ifndef DNF_MSG_DIE_ASSAULT_PLAYER_H
#define DNF_MSG_DIE_ASSAULT_PLAYER_H

#pragma pack(1)
struct MSG_DIE_ASSAULT_PLAYER
{
    int m_damage;               // +0x00（本回合累计伤害；==1 时 addServerHackCnt 0x7d）
    unsigned int m_attackCount; // +0x04（damage/attackCount >100000 → addServerHackCnt 0x7e）
    short m_field08;            // +0x08（DiePvP 日志第 3 参）
    short m_field0a;            // +0x0a（DiePvP 第 4 参）
    short m_field0c;            // +0x0c（DiePvP 第 5 参）
    short m_field0e;            // +0x0e（DiePvP 第 6 参）
};                              // sizeof == 0x10
#pragma pack()
#endif  // DNF_MSG_DIE_ASSAULT_PLAYER_H
