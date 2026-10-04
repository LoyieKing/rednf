char CUserCharacInfo::getCurCharFirstGrowType() const
{
    return m_selected ? (char)m_selected->m_growthType.m_sub : 0;
}
