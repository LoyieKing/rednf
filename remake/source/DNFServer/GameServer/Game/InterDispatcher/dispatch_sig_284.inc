int Inter_HadesAction::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_HadesActionPktView* pPkt = reinterpret_cast<Inter_HadesActionPktView*>(data);

    if (user != 0 && user->get_acc_id() == pPkt->m_a)
    {
        char ipBuf[16] = { '\0' };                     // 16 字节内联清零
        static_cast<XNuclear::CHades*>(user->getHades())
            ->GetIP(ipBuf);                            // 两次 getHades 各取一次（P48）
        static_cast<XNuclear::CHades*>(user->getHades())
            ->RunHadesAction(pPkt->m_12,
                             pPkt->m_16,
                             pPkt->m_1a,
                             pPkt->m_1e,
                             ipBuf);
        send_alert(user, pPkt->m_12);
    }
    else if (user == 0 && pPkt->m_a != 0)
    {
        if ((pPkt->m_12 & 2) != 0)
        {   // 双端惩罚（type=2）
            DB_HadesPunish::makeRequest(
                pPkt->m_a, 2,
                pPkt->m_16,
                pPkt->m_1a,
                pPkt->m_1e,
                reinterpret_cast<char*>(0));
        }
        if ((pPkt->m_12 & 1) != 0)
        {   // 单端惩罚（type=1）
            DB_HadesPunish::makeRequest(
                pPkt->m_a, 1,
                pPkt->m_16,
                pPkt->m_1a,
                pPkt->m_1e,
                reinterpret_cast<char*>(0));
        }
    }
    return 0;
}