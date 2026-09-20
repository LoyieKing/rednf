bool CUserCharacInfo::IsCurCharacVisible()
{
    if (m_selected && (m_selected->m_tagVisibleFlags.m_flag & 1))
        return true;
    return false;
}