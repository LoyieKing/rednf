// [b28d-solo-PvP_Room] P248 档①/P292: 返回 int→bool（ORIG set_ready_state 0x085d8368 调用点 `call; xor $1,%eax; test %al,%al; je`）；mangled 不变、自身 AE
bool PvP_Room::check_team_pvp()
{
    // [b28d-7] ORIG 头部 0x085da6xx：`cmp $5; je BODY; mov$1; jmp END; BODY:`
    // ⇒ 正条件 goto + return 1 fall-through（if 包裹形反产 jne）；
    // 尾部 `je RET1; mov$0; jmp; RET1: mov$1` ⇒ P47 三目形。
    if (((m_pvpBattleMode == 2) || (m_pvpBattleMode == 3)) || (m_pvpBattleMode == 5))
        goto CT_PVP_BODY;
    return 1;
CT_PVP_BODY:
    {
        if (G_GameWorld()->GetChannelType() == 7)
        {
            return 1;
        }
        int local_18 = 0;
        int local_14 = 0;
        for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
        {
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
            return 0;
        return 1;
    }
}