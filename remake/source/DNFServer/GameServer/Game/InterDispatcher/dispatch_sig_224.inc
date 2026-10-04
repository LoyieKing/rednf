int Inter_SetSecounUserData::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_SetSecounUserDataPktView* pPkt = reinterpret_cast<Inter_SetSecounUserDataPktView*>(data);
    user->gainMoneyFromHackUser(
        pPkt->m_who,
        pPkt->m_gold,
        pPkt->m_cnt,
        pPkt->m_totalGold);
    return 0;
}