unsigned char CUserCharacInfo::GetOneDayLetheFlag() const
{
    return m_selected ? m_selected->m_padEB3 : 0;
}