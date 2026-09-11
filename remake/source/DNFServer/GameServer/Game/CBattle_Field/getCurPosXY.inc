// N81 附族：m_curPos[m_curSeatNo] 索引段 `mov mem,%eax`+`lea` 合成 vs ORIG
// `mov mem,%edx`+`add` 直落（同 GetCurPosX/Y，4.1 索引 regmove 落点，合法源码
// 不可复现——负面结论同列）。
// [b28d-solo-CBattle_Field] N4/P30 同 GetCurPosX（51=51，+2 索引段两处；ORIG 0x08300da6）。
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
}