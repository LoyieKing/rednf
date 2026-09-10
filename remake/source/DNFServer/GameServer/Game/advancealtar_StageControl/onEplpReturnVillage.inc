void StageControl::onEplpReturnVillage()
{
    if (reinterpret_cast<ProcStage*>(m_procStage)->getState() ==
        ProcStage::State::S_N6)
    {
        leaveUser();
    }
