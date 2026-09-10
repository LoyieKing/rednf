int CParty::ReviveUser(CUser* actor, unsigned short targetUid)
{
    if (get_state() != 2)
    {
        if (GetEPLPState() == 0)
        {
            return 0x13;
        }
    }

    CUser* dead = 0;
    for (int i = 0; i <= 3; i++)
    {
        if (!_checkValidUser(i))
        {
            continue;
        }
        if (m_member[i].m_pUser->get_unique_id() == targetUid)
        {
            if (m_battleData.m_arr54[i] != 0)
            {
                return 0x12;
            }
            dead = m_member[i].m_pUser;
            break;
        }
    }
    if (dead == 0)
    {
        return 0x15;
    }

    bool freeRevival = checkFreeRevivalCondition(0);   // -0x11 物化槽
    for (int j = 0; j <= 3; j++)
    {
        if (!_checkValidUser(j))
        {
            continue;
        }
        if (m_member[j].m_pUser == actor)
        {
            if (freeRevival != 0)
            {
                goto coin_skip;
            }
            if (m_battleData.m_arr7e8[j] == 0)
            {
                return 1;
            }
            HistoryLog::WriteUseCoin(
                reinterpret_cast<_IO_FILE*>(actor->m_field796f8),
                reinterpret_cast<CUserCharacInfo*>(dead)->getCurCharacName());
            --m_battleData.m_arr7e8[j];
        coin_skip:;
        }
    }
    set_charac_live(dead, static_cast<ENUM_USER_DUNGEON_STATE>(1));
    Reset_party_overlapped_drop_ratio();
    return 0;
