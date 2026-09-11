// ORIG 0x08461cb4：m_itemCnt=0；10 个 Inven_Item reset 后 itemIdx(+0x02) 置 -1。
void CDeathTower::CPlayData::stRewardItem_t::reset()
{
    m_itemCnt = 0;
    for (int i = 0; i < 10; ++i)
    {
        m_items[i].reset();
        m_items[i].m_addInfo = -1;
    }
}