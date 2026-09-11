// [b28d-solo-PvP_Room] P210 定义前置：caller 置于 IsPvpObserver 定义之前 ⇒ 4.4 保守
// outgoing（帧 $0x28、槽基 -0xc），与 ORIG 0x085d6774 一致；旧 P190 pads 形只撑帧
// 不改槽基（-0x4 起）已废。P169 外层正向 if 包内层 goto 使三 je 共落 BODY。
int PvP_Room::get_pvp_team_cnt()
{
    int local_18;
    int local_14;
    if (((m_pvpBattleMode == 2) || (m_pvpBattleMode == 3)) || (m_pvpBattleMode == 5))
        goto TEAM_BODY;
    return -1;
TEAM_BODY:
    {
        local_18 = 0;
        local_14 = 0;
        for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
        {
            // [b28d-7] P214①：ORIG 正条件 bool 物化（mov$1/jmp/mov$0）+失败跳尾
            // ⇒ 源形=直接 && 包裹（if/else 两臂赋值反产 movb/cmpb）。
            if ((P_SEATS(local_10) != 0) && (!IsPvpObserver(local_10)))
            {
                if (P_TEAMS(local_10) == 1)
                {
                    local_18 = local_18 + 1;
                }
                else if (P_TEAMS(local_10) == 2)
                {
                    local_14 = local_14 + 1;
                }
            }
        }
        if (local_14 != local_18)
        {
            return -1;
        }
    }
    return local_14;
}