void CUserCharacInfo::addCurCharacExp(int exp)
{
    enableSaveCharacStat();
    if (m_selected)
    {
    m_selected->m_field2b += exp;
    }
}