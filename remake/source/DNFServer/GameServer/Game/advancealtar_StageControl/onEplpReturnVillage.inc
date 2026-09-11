// ORIG T 0x08130692（onEplpReturnVillage.md 全文）：ProcStage::getState()==6 → leaveUser()。
void StageControl::onEplpReturnVillage()
{
    if (reinterpret_cast<ProcStage*>(m_procStage)->getState() ==
        ProcStage::State::S_N6)
    {
        leaveUser();
    }
}