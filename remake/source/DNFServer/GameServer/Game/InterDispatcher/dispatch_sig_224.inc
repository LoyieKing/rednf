int Inter_SetSecounUserData::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_SetSecounUserDataPktView* pPkt = reinterpret_cast<Inter_SetSecounUserDataPktView*>(data);
    user->gainMoneyFromHackUser(
        pPkt->m_14,
        pPkt->m_0,
        pPkt->m_10,
        pPkt->m_8);
    return 0;
