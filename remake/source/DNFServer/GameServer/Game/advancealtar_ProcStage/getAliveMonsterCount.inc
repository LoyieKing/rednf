// ORIG W 0x08134820（objdump）：return m_field2c - m_field28。
int ProcStage::ProcSummonObjectMs::getAliveMonsterCount() const
{
    return m_aliveDelta - m_deadCount;
}