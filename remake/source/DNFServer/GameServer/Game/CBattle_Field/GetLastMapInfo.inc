MapInfo* CBattle_Field::GetLastMapInfo()
{
    int x = -1;
    int y = -1;
    if (m_pDungeon != 0 && m_pDungeon->IsEnterEachMap())
    {
        x = m_lastPos[m_curSeatNo].first;
        y = m_lastPos[m_curSeatNo].second;
    }
    else
    {
        x = m_lastX;
        y = m_lastY;
    }
    return GetMapInfoFromPos(x, y, false);
