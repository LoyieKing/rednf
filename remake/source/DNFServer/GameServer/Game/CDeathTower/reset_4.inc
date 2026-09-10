void CDeathTower::CPlayData::stRewardItem_t::reset()
{
    m_itemCnt = 0;
    for (int i = 0; i < 10; ++i)
    {
        m_items[i].reset();
        m_items[i].m_addInfo = -1;
    }
