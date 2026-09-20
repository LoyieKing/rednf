char CUserCharacInfo::getCurCharacGrowType() const
{
    return m_selected ? (char)m_selected->m_field29.m_growthType : 0;
}
