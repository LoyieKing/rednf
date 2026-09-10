void CBattle_Field::getCurPosXY(int& x, int& y)
{
    if (m_pDungeon != 0 && m_pDungeon->IsEnterEachMap() &&
        m_curSeatNo >= 0 && m_curSeatNo < 4)
    {
        x = m_curPos[m_curSeatNo].first;
        y = m_curPos[m_curSeatNo].second;
    }
    else
    {
        x = m_curX;
        y = m_curY;
    }
