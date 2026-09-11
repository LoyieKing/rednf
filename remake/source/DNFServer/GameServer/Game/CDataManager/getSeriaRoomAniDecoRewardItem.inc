int CDataManager::getSeriaRoomAniDecoRewardItem(int idx)
{
    if (m_seriaAniDeco.m_rewardList.empty())
    {
        return 0;
    }
    std::vector<seriaRoomDecoRewardInfo>::iterator it =
        m_seriaAniDeco.m_rewardList.begin();
    std::vector<seriaRoomDecoRewardInfo>::iterator e =
        m_seriaAniDeco.m_rewardList.end();
    while (it != e)
    {
        if (it->m_itemIdx == idx)
        {
            return it->m_reward;
        }
        it++;
    }
    return 0;
}