int Quest::get_title_reward() const
{
    if (m_type != 7)
    {
        return 0;
    }
    if (m_titleReward.size() == 0)
    {
        return 0;
    }
    return m_titleReward[0].m_itemIdx;
}