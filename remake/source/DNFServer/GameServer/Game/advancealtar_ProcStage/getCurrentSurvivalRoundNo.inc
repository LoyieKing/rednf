// ORIG W 0x08134878：return *(int*)(this+0x7c) + 1（objdump 实证 add $0x1）。
int ProcStage::getCurrentSurvivalRoundNo() const
{
    return m_survivalRound + 1;
}