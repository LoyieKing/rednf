void stFavorableRelationShip_t::stRewardItemInfo_t::clear()
{
    for (int j = 0; j <= 2; ++j)
    {
        m_byDir[j].clear();
        m_weight[j] = 0;
    }
