char CUserCharacInfo::getCurCharacGrowType() const
{
    return m_selected ? (char)m_selected->m_growthType.m_growthType : 0;
}
