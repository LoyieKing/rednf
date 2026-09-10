int CBattle_Field::GetCurPosX()
{
    if (m_pDungeon != 0 && m_pDungeon->IsEnterEachMap())
    {
        // ORIG 0x085bf364-0x373 索引段 5 条：`mov 0x114(%eax),%edx`（值直落
        // edx）+ `add $0x2,%edx` 原地自增。4.4 恒 `mov mem,%eax` 落点 +
        // `lea` 合成（实测 P168 值绑定 register asm("edx") 反产 mov%eax,%edx
        // +回读 6 条，负结论已回报），合法源码无法复现直落形——记 N81 附族
        // （4.1 索引 regmove/load 落点），行集仍 5 条。
        return m_curPos[m_curSeatNo].first;
    }
    else
    {
        return m_curX;
    }
