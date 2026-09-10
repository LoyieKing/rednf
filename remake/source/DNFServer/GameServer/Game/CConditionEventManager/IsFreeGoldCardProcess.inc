bool CConditionEventManager::IsFreeGoldCardProcess(
    std::vector<stConditionEventRewardInfo>& infos)
{
    for (unsigned int i = 0; i < infos.size(); ++i)
    {
        if (infos[i].m_flag8)
            return true;
    }
    return false;
