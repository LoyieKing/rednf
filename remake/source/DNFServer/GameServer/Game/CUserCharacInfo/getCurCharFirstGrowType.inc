char CUserCharacInfo::getCurCharFirstGrowType() const
{
    return m_selected ? (char)CUR->m_growType.first : 0;
}