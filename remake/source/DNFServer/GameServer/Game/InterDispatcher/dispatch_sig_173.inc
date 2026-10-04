// [b28d-lane2] verdict=DIFF ndiff=38（ORIG 0x084d4f46）：混合残差；gap+5。
// [b28d-solo-InterDispatcher] verdict=AE ndiff=0（本轮修：P227 全表达式临时链 + 删 (int)
// 冗余 cast + SetPowerWarUserRankingInfo 实参改 `&pPkt->m_ranker1`（走槽读））。
int Inter_NoticePowerWarRank::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_NoticePowerWarRankPktView* pPkt = reinterpret_cast<Inter_NoticePowerWarRankPktView*>(data);
    // P227：ORIG 在 ctor 前把三字段载入 %edi/%esi/%ebx 跨 ctor 保活 ⇒ 全表达式临时链。
    cMyTrace(__PRETTY_FUNCTION__, 0x368e, 0)(
        "RECV Packet_Notice_Power_War_Rank ranker 1:%d, 2:%d, 3:%d",
        pPkt->m_ranker1,
        pPkt->m_ranker2,
        pPkt->m_ranker3);
    GlobalData::s_power_manager->SetPowerWarUserRankingInfo(
        reinterpret_cast<unsigned int*>(&pPkt->m_ranker1));
    return 0;
}