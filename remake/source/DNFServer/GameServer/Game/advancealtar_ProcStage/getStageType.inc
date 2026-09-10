int ProcStage::getStageType() const
{
    if (m_dungeon == 0)
    {
        return 0;
    }
    // CDungeon+0x804 = m_advanceAltarType（int；getStageType.md 汇编 `mov 0x804(%eax),%eax`）。
    return m_dungeon->m_advanceAltarType;
