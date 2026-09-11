// ORIG W 0x0812eb4e（getClearSurvivalRoundNo.md objdump）：return *(this+0x7c)。
int ProcStage::getClearSurvivalRoundNo() const
{
    return m_survivalRound;
}