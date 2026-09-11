bool CUserCharacInfo::IsCurCharacVisible()
{
    if (m_selected && CUR->m_visibleBits.visible)
        return true;
    return false;
}