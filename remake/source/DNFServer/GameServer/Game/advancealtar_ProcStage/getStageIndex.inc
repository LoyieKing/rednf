// ORIG T 0x0812eb26：dungeon 非空 ? dungeon->get_index() : 0。
int ProcStage::getStageIndex() const
{
    if (m_dungeon == 0)
    {
        return 0;
    }
    return m_dungeon->get_index();
}