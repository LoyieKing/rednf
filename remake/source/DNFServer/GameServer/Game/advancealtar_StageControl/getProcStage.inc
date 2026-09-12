// ORIG W 0x081348ae：return (ProcStage*)(this + 0x04)。
// [BigP3D 成员化 2026-09-12] m_procStage 由字节槽改真实成员 ⇒ 取址 `&m_procStage`
// （等价 this+0x04；sizeof/偏移/mangled 零变）。
ProcStage* StageControl::getProcStage()
{
    return &m_procStage;
}