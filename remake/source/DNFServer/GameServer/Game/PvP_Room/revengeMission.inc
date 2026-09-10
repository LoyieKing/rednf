// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::revengeMission（原 TU 第 2600 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

void PvP_Room::revengeMission(bool flag)
{
    CMissionList_Charac* mission;
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        if (P_SEATS(local_10) != 0)
        {
            mission = (CMissionList_Charac*)P_SEATS(local_10)->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
            // ORIG 085ddf7f: mov 0x8(%ebp),%eax / mov 0x4(%eax),%edx —— 成员值单步
            // 直落 %edx（技巧2 register 直落型），再统一 spill 到 0x8(%esp)。
            // 4.4 对「成员→%eax 后再挪 %edx 槽」恒物化两步（N18a 族：4.1 可单步
            // 直落任意硬寄存器，4.4 fold 仅到首寄存器），register asm 反产更差形。
            // [batch-19 复核] `static_cast`↔C 式 `(PVP_BATTLE_MODE)` 两种写法实测
            // 输出全等（45 insn），中转步与 cast 形式无关 ⇒ N18a 封顶，ndiff=3。
            mission->Update_Revenge_event(*P_SEATS(local_10),
                                          static_cast<PVP_BATTLE_MODE>(m_pvpBattleMode),
                                          flag);
        }
    }
}
