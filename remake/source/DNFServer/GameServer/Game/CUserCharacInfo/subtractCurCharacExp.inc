void CUserCharacInfo::subtractCurCharacExp(int exp)
{
    enableSaveCharacStat();
    if (m_selected)
    {
    m_selected->m_field2b -= exp;
    }
}