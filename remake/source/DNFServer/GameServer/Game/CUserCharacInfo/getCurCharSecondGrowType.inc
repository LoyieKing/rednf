char CUserCharacInfo::getCurCharSecondGrowType() const
{
    return m_selected ? (char)m_selected->m_growthType.m_grow : 0;
}
