// N81 附族：m_lastPos[m_curSeatNo] 索引段同上（add vs lea 直落，不可复现）。
// [b28d-solo-CBattle_Field] N4/P30 同族（47=47，+6 索引段两处；ORIG 0x08300e44）。
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
}