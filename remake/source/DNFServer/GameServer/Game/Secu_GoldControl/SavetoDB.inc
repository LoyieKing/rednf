void Secu_GoldControl::SavetoDB(bool a, bool b, bool c)
{
    if (a && (m_cur.m_count[2] != 0 || m_cur.m_count[4] != 0 || m_fieldb8 != 0))
    {
        char ipBuf[16] = {0};
        ((XNuclear::CHades*)m_pUser->getHades())->GetIP(ipBuf);

        char macBuf[65] = {0};
        // get mac address from CNetwork at this+0xe0
        strncpy(macBuf, m_pUser->m_network.get_mac_addr(), 0x41);

        if (c)
        {
            user_gold_info tmp;
            resetRangeMoney(m_cur, tmp);
        }
        else
        {
            m_cur.reset();
        }
        m_fieldb8 = 0;
    }

    if (b)
    {
        if (m_backup.m_count[2] != 0 || m_backup.m_count[4] != 0 || m_fieldbc != 0)
        {
            char ipBuf[16] = {0};
            ((XNuclear::CHades*)m_pUser->getHades())->GetIP(ipBuf);

            char macBuf[65] = {0};
            strncpy(macBuf, m_pUser->m_network.get_mac_addr(), 0x41);

            m_backup.reset();
            m_fieldbc = 0;
        }
    }
}