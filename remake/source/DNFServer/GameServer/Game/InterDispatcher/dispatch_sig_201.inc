int Inter_SetFactoryHubUserNotConn::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user != 0)
    {
    // ORIG：包基址槽在 user 非空门之后物化。
    Inter_SetFactoryHubUserNotConnPktView* pPkt = reinterpret_cast<Inter_SetFactoryHubUserNotConnPktView*>(data);
        user->getHackAnalyzer()->ReqDBSaveHackInfo(
            pPkt->m_0,
            pPkt->m_4,
            pPkt->m_8,
            (int)user->get_acc_id(), 1);
    }
    return 0;
}