int Inter_AutoPunishRuleHackType::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    GlobalData::g_autoPunishRuleHackTypeMgr->reset();
    Inter_AutoPunishRuleHackTypePktView* pPkt = reinterpret_cast<Inter_AutoPunishRuleHackTypePktView*>(data);
    GlobalData::g_autoPunishRuleHackTypeMgr->setTotalTrade(pPkt->m_0,
                       pPkt->m_4);

    // P328⑩/P227：ORIG 0x84d4484..0x84d44b0 先求 m_0→%edi、m_4 低→%ebx/高→%esi，
    // 其后才 cMyTrace ctor ⇒ 全表达式临时链。
    cMyTrace(__PRETTY_FUNCTION__, 0x3585, 0)(
        "HackType Load Start. (TotalGold: %llu)(TotalCount: %d)",
        pPkt->m_4,
        pPkt->m_0);

    for (int local_20 = 0;
         local_20 < (int)(unsigned int)(unsigned short)pPkt->m_c;
         ++local_20)
    {
        GlobalData::g_autoPunishRuleHackTypeMgr->setHackCnt(
            (WongWork::ENUM_HACKTYPE)((Inter_AutoPunishRuleHackTypeEntryWinView*)((char*)pPkt + local_20 * 0x14))->m_10,
            ((Inter_AutoPunishRuleHackTypeEntryWinView*)((char*)pPkt + local_20 * 0x14))->m_12,
            (WongWork::ENUM_HACKTYPE)((Inter_AutoPunishRuleHackTypeEntryWinView*)((char*)pPkt + local_20 * 0x14))->m_14,
            ((Inter_AutoPunishRuleHackTypeEntryWinView*)((char*)pPkt + local_20 * 0x14))->m_16,
            ((Inter_AutoPunishRuleHackTypeEntryWinView*)((char*)pPkt + local_20 * 0x14))->m_18,
            ((Inter_AutoPunishRuleHackTypeEntryWinView*)((char*)pPkt + local_20 * 0x14))->m_1c,
            ((Inter_AutoPunishRuleHackTypeEntryWinView*)((char*)pPkt + local_20 * 0x14))->m_20);
        // ORIG 0x84d45db..0x84d466f 先求 +0x16→-0x5c、+0x14→%edi、+0x12→%esi、+0x10→%ebx 再 ctor。
        cMyTrace(__PRETTY_FUNCTION__, 0x3596, 0)(
            "HackType Load (Type: %d -> %d)(SubType: %d -> %d)",
            (unsigned int)((Inter_AutoPunishRuleHackTypeEntryWinView*)((char*)pPkt + local_20 * 0x14))->m_10,
            (unsigned int)((Inter_AutoPunishRuleHackTypeEntryWinView*)((char*)pPkt + local_20 * 0x14))->m_12,
            (unsigned int)((Inter_AutoPunishRuleHackTypeEntryWinView*)((char*)pPkt + local_20 * 0x14))->m_14,
            (unsigned int)((Inter_AutoPunishRuleHackTypeEntryWinView*)((char*)pPkt + local_20 * 0x14))->m_16);
    }

    cMyTrace(__PRETTY_FUNCTION__,
        0x3599, 0)("HackType Load End");
    return 0;
