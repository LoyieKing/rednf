void CUserCharacInfo::setCurCharacExp(int exp)
{
    enableSaveCharacStat();
    if (m_selected)
    {
    CUR->m_exp = exp;
    }
