int Inter_NoticePowerWarRank::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_NoticePowerWarRankPktView* pPkt = reinterpret_cast<Inter_NoticePowerWarRankPktView*>(data);
    // P227：ORIG 在 ctor 前把三字段载入 %edi/%esi/%ebx 跨 ctor 保活 ⇒ 全表达式临时链。
    cMyTrace(__PRETTY_FUNCTION__, 0x368e, 0)(
        "RECV Packet_Notice_Power_War_Rank ranker 1:%d, 2:%d, 3:%d",
        pPkt->m_a,
        pPkt->m_e,
        pPkt->m_12);
    GlobalData::s_power_manager->SetPowerWarUserRankingInfo(
        reinterpret_cast<unsigned int*>(&pPkt->m_a));
    return 0;
