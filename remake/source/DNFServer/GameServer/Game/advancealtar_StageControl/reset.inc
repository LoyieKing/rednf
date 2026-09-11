// ORIG T 0x0812fa54（reset.md 全文）：ProcStage::resetAll() + CharacData::reset()。
void StageControl::reset()
{
    // ORIG 直访 this+0x04 / this+0x98（不进 getProcStage/getCharacData thunk 函数；P-O0 内联访问器直访）。
    reinterpret_cast<ProcStage*>(m_procStage)->resetAll();
    reinterpret_cast<CharacData*>(m_characData)->reset();
}