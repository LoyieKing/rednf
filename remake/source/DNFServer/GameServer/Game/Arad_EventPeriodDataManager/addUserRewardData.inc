void Arad_EventPeriodDataManager::addUserRewardData(
    CUser* user, arad::SigLoadRewardEventItem& sig)
{
    for (int i = 0; i < (int)sig.m_count; ++i)
    {
        EventRewardItemArad item(sig.m_eventCodes[i], user->get_acc_id(),
                                 sig.m_characNos[i]);
        if (isRewarded(item))
        {
            LogManager::logFormat(
                1, "localjapan/Arad_EventPeriodDataManager.cpp",
                "void ARAD::Arad_EventPeriodDataManager::addUserRewardData(CUser*, "
                "arad::SigLoadRewardEventItem&)",
                0xe3,
                "[FnReward] already reward item. (mid:%u, charac:%u, event:%d)",
                user->get_acc_id(), user->get_charac_no(-1), sig.m_eventCodes[i]);
        }
        else
        {
            addUserRewardData(user, item);
        }
    }
