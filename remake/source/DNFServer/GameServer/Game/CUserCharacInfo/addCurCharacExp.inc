void CUserCharacInfo::addCurCharacExp(int exp)
{
    enableSaveCharacStat();
    if (m_selected)
    {
    CUR->m_exp += exp;
    }
