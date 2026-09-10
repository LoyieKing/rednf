bool CParty::_checkValidUser(int idx)
{
    // b28d-49 判词：N 封顶（N82 帧/槽）。P167 CLEAR。@0x85b4d12
    if (m_member[idx].m_pUser != 0)
    {
        if ((int)m_member[idx].m_pUser->GetUID() == m_member[idx].m_field8)
        {
            if (reinterpret_cast<CUserCharacInfo*>(m_member[idx].m_pUser)->getCurCharacR() != 0)
            {
                return 1;
            }
        }
        int cState = (char)m_state;
        int pIndex = m_partyIndex;
        unsigned short dungIndex = m_dungIndex;
        CUser* manager = m_manager;
        cMyTrace log1(__PRETTY_FUNCTION__, 0x37cf, 5);
        log1(
            "[PARTYPOOL] m_pManager : %x, dungeon_idx : %u, m_iIndex : %u, cState : %d",
            manager, (unsigned int)dungIndex, pIndex, cState);
        for (int i = 0; i <= 3; ++i)
        {
            if (m_member[i].m_pUser == 0)
            {
                cMyTrace log2(__PRETTY_FUNCTION__, 0x37e5, 5);
                log2("User[%d] NULL uid : %d", i, m_member[i].m_field8);
            }
            else
            {
                cMyTrace log3(__PRETTY_FUNCTION__, 0x37da, 5);
                log3(
                    "User[%d] %x, ch_state %d, uid : %d / %d, charac_no : %u",
                    i, m_member[i].m_pUser, m_member[i].m_pUser->get_state(),
                    (int)m_member[i].m_pUser->GetUID(), m_member[i].m_field8,
                    m_member[i].m_pUser->getCurCharacNo());
            }
        }
        int uid = (int)m_member[idx].m_pUser->GetUID();
        unsigned int accId = m_member[idx].m_pUser->get_acc_id();
        char* accIdStr = NumberToString(accId, 0);
        cMyTrace log4(__PRETTY_FUNCTION__, 0x37ed, 5);
        log4("User missmatched in party (m_id: %s)(uid: %d)", accIdStr, uid);
        m_member[idx].m_pUser->DisConnSig((DISCONN_SIG)0x17, false, 0);
        m_member[idx].Init();
    }
    return 0;
