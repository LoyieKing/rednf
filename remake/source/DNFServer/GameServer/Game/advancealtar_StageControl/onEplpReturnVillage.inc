// ORIG T 0x08130692（onEplpReturnVillage.md 全文）：ProcStage::getState()==6 → leaveUser()。
void StageControl::onEplpReturnVillage()
{
    if (m_procStage.getState() ==   // [BigP3D 成员化] 真实成员直调（等价 this+0x04）
        ProcStage::State::S_N6)
    {
        leaveUser();
    }
}