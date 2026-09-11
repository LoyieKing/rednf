void CBattle_Field::CBloodClearRewardData::stRewardItem_t::reset()
{
    m_count = 0;
    for (int i = 0; i <= 9; ++i)
    {
        m_item[i].reset();
        m_item[i].m_addInfo = -1;
    }
}