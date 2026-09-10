void ProcStage::resetAll()
{
    m_dungeon = 0;
    m_startStageTime = 0;
    reset(StageType::STAGE_NORMAL);
    reset(StageType::STAGE_SURVIVAL);
