// ORIG T 0x0812fb98（onTimerStageTick.md 全文）：ProcStage::getState 为 3/4 时
//   CharacData::upGage() + ProcStage::procTimeLine()，恒返回 1。
bool StageControl::onTimerStageTick()   // 返回类型 bool（advancealtar.h:513 同源，StrC13 会签）
{
    if (reinterpret_cast<ProcStage*>(m_procStage)->getState() == ProcStage::State::S_N3 ||
        reinterpret_cast<ProcStage*>(m_procStage)->getState() == ProcStage::State::S_N4)
    {
        reinterpret_cast<CharacData*>(m_characData)->upGage();
        reinterpret_cast<ProcStage*>(m_procStage)->procTimeLine();
    }
    return true;  // ORIG 恒返回 1
}