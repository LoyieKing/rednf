void CUserCharacInfo::IncCurCharacLuckPointUpdateTimeCount()
{
    if (m_selected)
        m_selected->m_luckPointUpdateTimeCount += 1;
}