// ---- DimensionActivationRewardData 三查询（同名报告全文转写）---------------
EventRewardSection* DimensionActivationRewardData::getSectionData(int idx)
{
    for (std::vector<EventRewardSection>::reverse_iterator it = m_vec0.rbegin();
         it != m_vec0.rend(); ++it)
    {
        if ((int)(unsigned)it->m_field1 <= idx)
        {
            return &(*it);
        }
    }
    return 0;
}