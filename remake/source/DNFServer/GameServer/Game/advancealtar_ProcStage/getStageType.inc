// ORIG T 0x0812eb04：dungeon 非空 ? *(int*)(dungeon+0x804) : 0。
int ProcStage::getStageType() const
{
    if (m_dungeon == 0)
    {
        return 0;
    }
    // CDungeon+0x804 = m_advanceAltarType（int；getStageType.md 汇编 `mov 0x804(%eax),%eax`）。
    return m_dungeon->m_advanceAltarType;
}