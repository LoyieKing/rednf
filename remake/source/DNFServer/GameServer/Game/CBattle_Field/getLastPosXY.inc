void CBattle_Field::getLastPosXY(int& x, int& y)
{
    if (m_pDungeon->IsEnterEachMap() &&
        m_curSeatNo >= 0 && m_curSeatNo < 4)
    {
        x = m_lastPos[m_curSeatNo].first;
        y = m_lastPos[m_curSeatNo].second;
    }
    else
    {
        // ORIG 0x8300e44 原版笔误：两次均写 &x（mov 0xc(%ebp)），y 形参未写。
        x = m_lastX;
        x = m_lastY;
    }
