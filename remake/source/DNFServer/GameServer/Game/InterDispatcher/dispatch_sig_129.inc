int Inter_AutoPunishRuleHackType::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    GlobalData::g_autoPunishRuleHackTypeMgr->reset();
    SIG_AUTO_PUNISH_RULE_HACK_TYPE* rule = reinterpret_cast<SIG_AUTO_PUNISH_RULE_HACK_TYPE*>(data);
    GlobalData::g_autoPunishRuleHackTypeMgr->setTotalTrade(rule->m_field0,
                       ((unsigned long long)rule->m_field4 | ((unsigned long long)rule->m_field8 << 32)));

    // P328⑩/P227：ORIG 0x84d4484..0x84d44b0 先求 m_0→%edi、m_4 低→%ebx/高→%esi，
    // 其后才 cMyTrace ctor ⇒ 全表达式临时链。
    cMyTrace(__PRETTY_FUNCTION__, 0x3585, 0)(
        "HackType Load Start. (TotalGold: %llu)(TotalCount: %d)",
        ((unsigned long long)rule->m_field4 | ((unsigned long long)rule->m_field8 << 32)),
        rule->m_field0);

    for (int local_20 = 0;
         local_20 < (int)(unsigned int)(unsigned short)rule->m_count;
         ++local_20)
    {
        GlobalData::g_autoPunishRuleHackTypeMgr->setHackCnt(
            (WongWork::ENUM_HACKTYPE)G2_HKT_SUB(m_hackType),
            G2_HKT_SUB(m_cnt),
            (WongWork::ENUM_HACKTYPE)G2_HKT_SUB(m_hackSubType),
            G2_HKT_SUB(m_hackSubCnt),
            G2_HKT_SUB(m_applyFlag),
            // ORIG `add $0x1c,%eax; mov (%eax),%eax`（直接指针算术，非 disp 折叠）
            rule->m_entries[local_20].m_ipCnt,
            rule->m_entries[local_20].m_etcLo);
        // ORIG 0x84d45db..0x84d466f 先求 +0x16→-0x5c、+0x14→%edi、+0x12→%esi、+0x10→%ebx 再 ctor。
        cMyTrace(__PRETTY_FUNCTION__, 0x3596, 0)(
            "HackType Load (Type: %d -> %d)(SubType: %d -> %d)",
            (unsigned int)G2_HKT_SUB(m_hackType),
            (unsigned int)G2_HKT_SUB(m_cnt),
            (unsigned int)G2_HKT_SUB(m_hackSubType),
            (unsigned int)G2_HKT_SUB(m_hackSubCnt));
    }

#undef G2_HKT_SUB

    cMyTrace(__PRETTY_FUNCTION__,
        0x3599, 0)("HackType Load End");
    return 0;
}