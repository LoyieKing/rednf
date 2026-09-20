void CUserCharacInfo::SetOneDayLetheFlag2ND(unsigned char flag)
{
    if (m_selected)
    {
    m_selected->m_padEB5 = flag;
    }
}