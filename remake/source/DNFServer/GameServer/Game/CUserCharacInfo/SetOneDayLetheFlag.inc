void CUserCharacInfo::SetOneDayLetheFlag(unsigned char flag)
{
    if (m_selected)
    {
    m_selected->m_padEB3 = flag;
    }
}