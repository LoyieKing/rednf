int Inter_SetFactoryHubUserNotConn::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user != 0)
    {
    // ORIG：包基址槽在 user 非空门之后物化。
    SIG_FIND_FACTORY_HUB_USER_NO_CONN* hub = reinterpret_cast<SIG_FIND_FACTORY_HUB_USER_NO_CONN*>(data);
        user->getHackAnalyzer()->ReqDBSaveHackInfo(
            hub->m_mId,
            hub->m_characNo,
            hub->m_flag,
            (int)user->get_acc_id(), 1);
    }
    return 0;
}