int ProcStage::getStageIndex() const
{
    if (m_dungeon == 0)
    {
        return 0;
    }
    return m_dungeon->get_index();
