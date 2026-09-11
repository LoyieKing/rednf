int PvP_Room::set_pvp_mode(CUser* user, int mode)
{
    if (m_state != 1)
    {
        return 0x13;
    }
    if (m_seats[7] != user)
    {
        return 8;
    }
    lock();
    int iVar1 = m_pvpBattleMode;
    // ORIG 085d84ee: arg 先载（%edx）再 this（%eax）写回。N76 异型化翻案
    // （NL7/NL8 新定律）：m_pvpBattleMode int→unsigned int（private，正数
    // == 比较，无符号性语义差），4.4 异型 store 出实参先载形，ndiff 2→0。
    m_pvpBattleMode = mode;
    if (mode == 1)
    {
        for (int local_30 = 0; local_30 < 8; local_30 = local_30 + 1)
        {
            if (P_SEATS(local_30) != 0)
            {
                P_TEAMS(local_30) = 0;
                m_relayMgr.OnChangeTeam(local_30, 0);
            }
        }
    }
    else if (mode == 3)
    {
        if (iVar1 == 2)
        {
            for (int local_2c = 0; local_2c < 8; local_2c = local_2c + 1)
            {
                if (P_SEATS(local_2c) != 0)
                {
                    m_relayMgr.OnChangeTeam(local_2c, P_TEAMS(local_2c));
                }
            }
        }
        else if (iVar1 == 1)
        {
            int local_28 = 1;
            for (int local_24 = 0; local_24 < 8; local_24 = local_24 + 1)
            {
                if (P_SEATS(local_24) != 0)
                {
                    P_TEAMS(local_24) = local_28;
                    m_relayMgr.OnChangeTeam(local_24, P_TEAMS(local_24));
                    if (local_28 == 1)
                    {
                        local_28 = 2;
                    }
                    else
                    {
                        local_28 = 1;
                    }
                }
            }
        }
    }
    else if (mode == 4)
    {
        for (int local_20 = 0; local_20 < 8; local_20 = local_20 + 1)
        {
            if (P_SEATS(local_20) != 0)
            {
                P_TEAMS(local_20) = 0;
            }
        }
    }
    else if (mode == 5)
    {
        int local_1c = 1;
        for (int local_18 = 0; local_18 < 8; local_18 = local_18 + 1)
        {
            if (P_SEATS(local_18) != 0)
            {
                P_TEAMS(local_18) = local_1c;
                m_relayMgr.OnChangeTeam(local_18, local_1c);
                if (local_1c == 1)
                {
                    local_1c = 2;
                }
                else
                {
                    local_1c = 1;
                }
            }
        }
    }
    else if (iVar1 != 3)
    {
        int local_14 = 1;
        for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
        {
            if (P_SEATS(local_10) != 0)
            {
                P_TEAMS(local_10) = local_14;
                m_relayMgr.OnChangeTeam(local_10, P_TEAMS(local_10));
                if (local_14 == 1)
                {
                    local_14 = 2;
                }
                else
                {
                    local_14 = 1;
                }
            }
        }
    }
    unlock();
    return 0;
}