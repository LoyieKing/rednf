int CBattle_Field::GetCurPosY()
{
    if (m_pDungeon != 0 && m_pDungeon->IsEnterEachMap())
    {
        // 同上（ORIG 0x085bf3ba 区同构，N81 附族标注）。
        return m_curPos[m_curSeatNo].second;
    }
    else
    {
        return m_curY;
    }
