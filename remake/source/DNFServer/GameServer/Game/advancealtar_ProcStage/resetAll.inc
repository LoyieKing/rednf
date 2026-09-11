// ORIG T 0x0812dd9a：dungeon=0、startStageTime=0、reset(0)、reset(1)。
void ProcStage::resetAll()
{
    m_dungeon = 0;
    m_startStageTime = 0;
    reset(StageType::STAGE_NORMAL);
    reset(StageType::STAGE_SURVIVAL);
}