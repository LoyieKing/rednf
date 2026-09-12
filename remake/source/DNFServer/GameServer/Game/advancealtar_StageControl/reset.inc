// ORIG T 0x0812fa54（reset.md 全文）：ProcStage::resetAll() + CharacData::reset()。
void StageControl::reset()
{
    // ORIG 直访 this+0x04 / this+0x98（不进 getProcStage/getCharacData thunk 函数；P-O0 内联访问器直访）。
    // [BigP3D 成员化 2026-09-12] 真实成员直调 ⇒ 同上直访形态。
    m_procStage.resetAll();
    m_characData.reset();
}