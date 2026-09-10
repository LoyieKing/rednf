void CUserCharacInfo::IncCurCharacLuckPointUpdateTimeCount()
{
    if (m_selected)
        CUR->m_luckPointUpdateTimeCount += 1;
