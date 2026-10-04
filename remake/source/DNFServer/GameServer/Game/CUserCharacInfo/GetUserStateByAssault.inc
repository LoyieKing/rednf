int CUserCharacInfo::GetUserStateByAssault()
{
    if (m_selected != 0)
    {
        if (m_selected->m_superState != 0)
            return 6;
        if (m_selected->m_ghost != 0)
            return 4;
        if (m_selected->m_assaultPlace != 0)
            return 5;
    }
    return 0;
}