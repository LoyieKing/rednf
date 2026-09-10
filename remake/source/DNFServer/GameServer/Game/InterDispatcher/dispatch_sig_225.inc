int Inter_SetARSInfo::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_SetARSInfoPktView* pPkt = reinterpret_cast<Inter_SetARSInfoPktView*>(data);
    if (pPkt->m_e == 0)
    {
        GlobalData::g_autoPunishRuleHackTypeMgr->reset();
        cMyTrace(__PRETTY_FUNCTION__, 0x4f5e, 0)("[ARS_INFO] Reset");
    }

    cMyTrace(__PRETTY_FUNCTION__, 0x4f64, 0)("[ARS_INFO] Cnt : %d, State : %d",
          (int)pPkt->m_a, (int)(signed char)pPkt->m_e);

    if ((int)pPkt->m_a > 100)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x4f6b, 0)("[ARS_INFO] Cnt OverFlag(100)");
        return 0;
    }

    for (int i = 0; i < (int)pPkt->m_a; ++i)
    {
        if (((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_f == 0)
        {   // 交易总量条目：type=+0x11，total(u64)=+0x19
            GlobalData::g_autoPunishRuleHackTypeMgr->setTotalTrade(
                (unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_11,
                (unsigned long long)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_19);
            cMyTrace(__PRETTY_FUNCTION__, 0x4f82, 0)("[ARS_INFO] TotalGold: %llu, TotalCount: %d",
              (unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_19,
              (unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_11);
        }
        else
        {   // hack 计数条目：main=+0xf val=+0x11 sub=+0x13 val=+0x15 flag=+0x17 ipCnt=+0x18 arg=+0x19
            GlobalData::g_autoPunishRuleHackTypeMgr->setHackCnt(
                (WongWork::ENUM_HACKTYPE)(unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_f,
                (unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_11,
                (WongWork::ENUM_HACKTYPE)(unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_13,
                (unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_15,
                (unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_17,
                (unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_18,
                (unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_19);
            cMyTrace(__PRETTY_FUNCTION__, 0x4f8e, 0)("[ARS_INFO] MainType : %d (%d), SubType : %d(%d), ApplyFlag %d, IpCnt : %d",
              (unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_f,
              (unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_11,
              (unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_13,
              (unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_15,
              (unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_17,
              (unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_18);
        }
    }
    return 0;
