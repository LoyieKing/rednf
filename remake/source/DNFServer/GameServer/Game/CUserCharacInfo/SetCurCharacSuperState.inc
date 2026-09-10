void CUserCharacInfo::SetCurCharacSuperState(bool flag)
{
    if (m_selected)
    {
    CUR->m_superState = flag;
    }
