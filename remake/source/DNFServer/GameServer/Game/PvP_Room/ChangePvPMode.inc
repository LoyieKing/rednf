// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::ChangePvPMode（原 TU 第 2624 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

// [b28d-solo-PvP_Room] AE: P248 档①/P292 已落 —— IsDeathMatchMap 返回类型 bool 化 +
// PVPMAP_DEATHMATCH_MODE 改 bool 数组 ⇒ 调用点 `call; test %al; je` 无 setne，与 ORIG
// 0x085d8b2a 全等（旧「uchar callee setne 归一封顶」判词作废）。
int PvP_Room::ChangePvPMode(int mapNo)
{
    bool local_d;
    local_d = false;
    // [b28d-solo-PvP_Room] ORIG 0x085d8b2a `call; test %al,%al; je` 直测 + then
    // 分支（mode!=4&&!=5 → 1→4/2→5）直落、else 分支（4→1/5→2）后置；去 cVar1
    // 局部使 out 参数槽对齐 -0x9。
    if (IsDeathMatchMap(mapNo, local_d))
    {
        if ((m_pvpBattleMode != 4) && (m_pvpBattleMode != 5))
        {
            if (m_pvpBattleMode == 1)
            {
                m_pvpBattleMode = 4;
            }
            else if (m_pvpBattleMode == 2)
            {
                m_pvpBattleMode = 5;
            }
        }
    }
    else
    {
        if (m_pvpBattleMode == 4)
        {
            m_pvpBattleMode = 1;
        }
        else if (m_pvpBattleMode == 5)
        {
            m_pvpBattleMode = 2;
        }
    }
}
