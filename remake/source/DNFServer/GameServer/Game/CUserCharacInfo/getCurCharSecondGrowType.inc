char CUserCharacInfo::getCurCharSecondGrowType() const
{
    return m_selected ? (char)m_selected->m_field29.m_grow : 0;
}
