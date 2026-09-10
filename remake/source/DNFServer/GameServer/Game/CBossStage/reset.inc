void CBossStage::reset()
{
    m_startTick = 0;
    m_endTick = 0;
    m_stageData.Clear();
    memset(m_useItemCount, 0, sizeof(m_useItemCount));
