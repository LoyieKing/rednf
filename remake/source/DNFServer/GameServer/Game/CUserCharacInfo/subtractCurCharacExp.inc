void CUserCharacInfo::subtractCurCharacExp(int exp)
{
    enableSaveCharacStat();
    if (m_selected)
    {
    CUR->m_exp -= exp;
    }
}