// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::recv_pvp_end_result_team（原 TU 第 905 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

// [b28d-7 N95] ORIG 0x085dc0f0：槽 -0x4/-0x8（帧 $0x18）vs 4.4 -0xc/-0x10（帧 $0x28）+ do-while 旋转点差；P167 CLEAR，N95 封顶。
// [b28d-solo-PvP_Room] NEAR(70=70): P210② IsRecv 定义迁入 ⇒ 帧 $0x18/槽 -0x4/-0x8
// 归位；第一循环合并守卫（je/jne 共落 L4f）、第二循环 continue 双岛（P223）复现
// ORIG 0x085dc0f0 结构；残余仅重定位/寄存器角色显示差。
int PvP_Room::recv_pvp_end_result_team(CUser* user) const
{
    // [b28d-solo-PvP_Room] ORIG 0x085dc0f0：i 双初始化（movl $0,-0x8 两次）、
    // 两段 for(i<=7) setle 尾测、team 存 -0x4；4.4 do-while+goto 形全帧漂移。
    int local_8 = 0;
    int local_4 = 0xff;
    for (local_8 = 0; local_8 <= 7; local_8 = local_8 + 1)
    {
        // [b28d-solo-PvP_Room] ORIG 0x085dc11b/0x085dc12c 两守卫 je/jne 共落
        // L4f（无岛）⇒ 合并短路条件形（两个独立 continue 会产 nop;jmp 岛）。
        if (P_SEATS(local_8) != 0)
        {
            if (P_SEATS(local_8) == user)
            {
                local_4 = P_TEAMS(local_8);
                break;
            }
        }
    }
    if (local_4 == 0xff)
    {
        return 0;
    }
    for (local_8 = 0; local_8 <= 7; local_8 = local_8 + 1)
    {
        if (P_SEATS(local_8) == 0)
            continue;
        if (P_TEAMS(local_8) != local_4)
            continue;
        if (m_recvFlagBC.IsRecv(local_8) != 1)
        {
            return 0;
        }
    }
    return 1;
}
