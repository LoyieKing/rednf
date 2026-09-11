// N81 附族：m_lastPos[m_curSeatNo] 索引段同上（add vs lea 直落，不可复现）。
// [b28d-solo-CBattle_Field] N4/P30 封顶（49=49，+6 索引段两处）：ORIG
// `mov 0x114(%eax),%edx; add $0x6,%edx` vs 4.4 `mov mem,%eax; lea 0x6(%eax),%edx`
// （ORIG 0x0830eac8 段）。
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
}