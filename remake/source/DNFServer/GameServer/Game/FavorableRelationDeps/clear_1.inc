void stFavorableRelationShip_t::stRewardItem_t::clear()
{
    for (int i = 0; i <= 4; ++i)
    {
        for (int j = 0; j <= 2; ++j)
        {
            m_slots[i][j].clear();
        }
    }
