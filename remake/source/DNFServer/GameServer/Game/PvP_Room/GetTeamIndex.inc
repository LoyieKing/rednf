int PvP_Room::GetTeamIndex(CUser* user)
{
    // ORIG 0x085dc6ee：for+jmp-check+break 岛；-0x4=游标、-0x8=结果。
    int local_8;
    int local_4;
    for (local_4 = 0; local_4 < 8; local_4 = local_4 + 1)
    {
        if (P_SEATS(local_4) == 0)
        {
            continue;
        }
        if (P_SEATS(local_4) == user)
        {
            break;
        }
    }
    if (local_4 == 8)
    {
        return -1;
    }
    if ((m_pvpBattleMode == 1) || (m_pvpBattleMode == 4))
    {
        local_8 = local_4;
    }
    else
    {
        local_8 = P_TEAMWIN(local_4);
    }
    return local_8;
}