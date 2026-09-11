// N81 附族：m_lastPos[...]/m_curPos[...] 索引段 + make_pair 传参同理（add vs lea 直落，
// 不可复现）。
// [b28d-solo-CBattle_Field] N4/P30 同族（76=76，+6/+2 索引段 + make_pair<int&,int&> 段；ORIG 0x0830878e）。
void CBattle_Field::swap_curpos_xy(int x, int y)
{
    if (m_pDungeon != 0 && m_pDungeon->IsEnterEachMap() &&
        m_curSeatNo >= 0 && m_curSeatNo < 4)
    {
        m_lastPos[m_curSeatNo] = m_curPos[m_curSeatNo];
        m_curPos[m_curSeatNo] = std::make_pair<int&, int&>(x, y);
    }
    else
    {
        m_lastX = m_curX;
        m_lastY = m_curY;
        m_curX = x;
        m_curY = y;
    }
}