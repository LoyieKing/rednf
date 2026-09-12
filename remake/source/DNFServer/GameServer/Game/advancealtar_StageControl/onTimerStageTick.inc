// ORIG T 0x0812fb98（onTimerStageTick.md 全文）：ProcStage::getState 为 3/4 时
//   CharacData::upGage() + ProcStage::procTimeLine()，恒返回 1。
bool StageControl::onTimerStageTick()   // 返回类型 bool（advancealtar.h:513 同源，StrC13 会签）
{
    // [BigP3D 成员化 2026-09-12] 真实成员直调（等价 this+0x04/this+0x98 直算，P183）。
    if (m_procStage.getState() == ProcStage::State::S_N3 ||
        m_procStage.getState() == ProcStage::State::S_N4)
    {
        m_characData.upGage();
        m_procStage.procTimeLine();
    }
    return true;  // ORIG 恒返回 1
}