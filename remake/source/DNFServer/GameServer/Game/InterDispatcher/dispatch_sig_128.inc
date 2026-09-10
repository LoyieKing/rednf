int Inter_AutoPunishRuleBlackIP::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_AutoPunishRuleBlackIPPktView* pPkt = reinterpret_cast<Inter_AutoPunishRuleBlackIPPktView*>(data);
    GlobalData::g_autoPunishRuleBlackIPMgr->reset();

    const unsigned short totalCount =                 // msg+0x0
        (unsigned short)pPkt->m_0;
    cMyTrace(__PRETTY_FUNCTION__,
        0x3561, 0)("Black IP Load Start (TotalCount: %d)", (int)totalCount);

    for (unsigned int i = 0; i < totalCount; ++i)     // cmp; seta
    {
        char ipStr[16] = { '\0' };
        unsigned int addrPart =                       // msg+i*8+4
            *reinterpret_cast<unsigned int*>(data + i * 8 + 4);
        unsigned int netIp = htonl(
            (unsigned int)*reinterpret_cast<unsigned char*>(data + i * 8 + 8));
        strncpy(ipStr, inet_ntoa(
                    *reinterpret_cast<in_addr*>(&(netIp |= addrPart))), 0x10);

        const bool added =
            GlobalData::g_autoPunishRuleBlackIPMgr->addBlackIP(
                (unsigned long)addrPart,
                *reinterpret_cast<unsigned char*>(data + i * 8 + 8),
                *reinterpret_cast<unsigned char*>(data + i * 8 + 9));
        if (added != 1)
        {
            LogManager::logFormat(
                1, "InterDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0x356d,
                "Black IP Load fail(StartIP:%s -> %d)", ipStr,
                (int)*reinterpret_cast<unsigned char*>(data + i * 8 + 9));
        }
        const unsigned char maxIp =
            *reinterpret_cast<unsigned char*>(data + i * 8 + 9);
        cMyTrace(__PRETTY_FUNCTION__,
            0x356e, 0)("Black IP Load (StartIP:%s -> %d)", ipStr, (int)maxIp);
    }

    const int finalCount =
        GlobalData::g_autoPunishRuleBlackIPMgr->getBlackIPCount();
    cMyTrace(__PRETTY_FUNCTION__,
        0x3570, 0)("Black IP Load End (TotalCount: %d)", finalCount);
    return 0;
