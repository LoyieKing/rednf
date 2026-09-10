int ProcStage::ProcSummonObjectMs::getAliveMonsterCount() const
{
    return m_aliveDelta - m_deadCount;
