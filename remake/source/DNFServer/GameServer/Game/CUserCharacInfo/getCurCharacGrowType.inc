unsigned char CUserCharacInfo::getCurCharacGrowType() const
{
    return m_selected ? CUR->m_growType.byte : 0;
