bool CDungeon::get_dimension_member_count(DimensionPartyCount& out) const
{
    // ORIG 0x0834d220：std::find 匹配 m_0/m_1，命中复制 m_2 返回 true
    std::vector<DimensionPartyCount>::const_iterator it = std::find(
        m_dimensionPartyCount.begin(), m_dimensionPartyCount.end(), out);
    if (it != m_dimensionPartyCount.end())
    {
        out.m_2 = it->m_2;
        return true;
    }
    return false;
}