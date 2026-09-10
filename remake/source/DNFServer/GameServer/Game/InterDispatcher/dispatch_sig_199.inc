int Inter_LoadPowerWarInfo::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_LoadPowerWarInfoPktView* pPkt = reinterpret_cast<Inter_LoadPowerWarInfoPktView*>(data);
    // P227：ORIG 在 ctor 前把三字段载入 %edi/%esi/%ebx 跨 ctor 保活 ⇒ 全表达式临时链。
    cMyTrace(__PRETTY_FUNCTION__, 0x483a, 0)(
        "Receive SIG_LOAD_POWER_WAR_INFO(Winner:%d, A:%d, B:%d)",
        (signed char)pPkt->m_8,
        pPkt->m_0,
        pPkt->m_4);
    GlobalData::s_power_manager->SetPowerInfo(
        (char)pPkt->m_8,
        pPkt->m_0,
        pPkt->m_4);
    return 0;
