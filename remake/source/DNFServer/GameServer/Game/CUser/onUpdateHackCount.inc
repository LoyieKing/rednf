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
        // ORIG：HackTypeInfo_t::HackTypeInfo_t 真实 ctor 清零（非 memset）。
        // 2026-09-10（G3CUser-B，声明顺序/槽位轴）：ORIG 0x0867fe0e
        //   `lea -0x28(%ebp),%eax; mov %eax,(%esp); call HackTypeInfo_t::HackTypeInfo_t()`
        //   （info 槽 **-0x28**）出现在 0x0867fe1e `call CNetwork::GetPeerIP2(ip=
        //   -0x38(%ebp), 0x10)` **之前**；O0 下先声明者得高地址槽 ⇒ 原码把 info
        //   写在 GetPeerIP2 之前（我方原顺序反置，info 落 -0x38、ip 落 -0x28，
        //   且 ctor 调用点被推到 GetPeerIP2 之后）。
        WongWork::HackTypeInfo_t info;
        m_network.GetPeerIP2(ip, 0x10);
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
