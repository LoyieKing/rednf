int CUserCharacInfo::GetUserStateByAssault()
{
    if (m_selected != 0)
    {
        if (CUR->m_superState != 0)
            return 6;
        if (CUR->m_ghost != 0)
            return 4;
        if (CUR->m_assaultPlace != 0)
            return 5;
    }
    return 0;
