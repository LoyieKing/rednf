void CUser::onUpdateHackCount(WongWork::ENUM_HACKTYPE type, int count,
                              unsigned int a, unsigned int b)
{
    m_mcap.incPoint(
        WongWork::CMCAPManager::getPointPerHackType(type) * count);
    if (m_mcap.getPoint() > 1000)
        setStdDropRate(100 - (int)(m_mcap.getPoint() / 100));

    char ip[16];
    if (!isHackUserTimer())
    {
        getHades()->AddHackTypeCnt((unsigned long)type, (unsigned long)count,
                                   a, b);
        getHackAnalyzer()->setUserP(this);
        m_network.GetPeerIP2(ip, 0x10);

        // ORIG：HackTypeInfo_t::HackTypeInfo_t 真实 ctor 清零（非 memset）。
        WongWork::HackTypeInfo_t info;
        if (GlobalData::g_autoPunishRuleHackTypeMgr->checkMaxHackCntOver(
                type,
                (unsigned int)getHackAnalyzer()->getServerHackAccumulatedCnt(type),
                getHackAnalyzer(), info,
                m_lookOuts1, m_ipCounters, m_lookOuts2, ip))
        {
            setHackUser((unsigned int)info.m_type1,
                        (unsigned int)getHackAnalyzer()->
                            getServerHackAccumulatedCnt(
                                (WongWork::ENUM_HACKTYPE)info.m_type1),
                        (unsigned int)info.m_type2,
                        (unsigned int)getHackAnalyzer()->
                            getServerHackAccumulatedCnt(
                                (WongWork::ENUM_HACKTYPE)info.m_type2),
                        info.m_value2, info.m_value3);
        }
        if (info.m_type2 == 3)
        {
            WongWork::HackTypeInfo_t info2;
            if (GlobalData::g_autoPunishRuleHackTypeMgr->checkMaxHackCntOver(
                    type,
                    (unsigned int)getHackAnalyzer()->getServerHackAccumulatedCnt(type),
                    getHackAnalyzer(), info2,
                    m_lookOuts1, m_ipCounters, m_lookOuts2, ip))
            {
                setHackUser((unsigned int)info2.m_type1,
                            (unsigned int)getHackAnalyzer()->
                                getServerHackAccumulatedCnt(
                                    (WongWork::ENUM_HACKTYPE)info2.m_type1),
                            (unsigned int)info2.m_type2,
                            (unsigned int)getHackAnalyzer()->
                                getServerHackAccumulatedCnt(
                                    (WongWork::ENUM_HACKTYPE)info2.m_type2),
                            info2.m_value2, info2.m_value3);
            }
        }
    }

    m_historyLog.HackDetected(
        (unsigned int)type, (unsigned int)count,
        (unsigned int)getHackAnalyzer()->getServerHackCnt(type),
        (unsigned int)getHackAnalyzer()->getServerHackAccumulatedCnt(type),
        a, b);
