// ORIG T 0x0812f74c：stageType==1 时 survivalFirst=0。
void ProcStage::setNotSurvivalModeFirstRound()
{
    if (getStageType() == 1)
    {
        m_survivalFirst = 0;
    }
}