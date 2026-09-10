void WarRoom::ReviveAll()
{
    int i = 0;
    while (i <= 5)
    {
        // ORIG 086bc46f：双空岛 086bc4d7(nop;jmp 086bc4db)/086bc4da(nop;fall)，
        // body 尾 jmp 086bc4db（同 PvP_Room check_recv_pvp_rank 已验证双 goto 模式）。
        if (m_slots[i] == 0)
            goto REVIVE_NEXT;
        if (m_liveFlag[i] != 0)
            goto REVIVE_SKIP;
        SetCharacterLive(i, m_slotsUser[i], true, false, 0, 0xff);
REVIVE_SKIP:
        ;
REVIVE_NEXT:
        ;
        ++i;
    }
