// ----------------------------------------------------------------------------
// ORIG T 0x0812f9a2（StageControl.md 全文）：m_index=-1 →
//   ProcStage::ProcStage(this+4, characData@this+0x98)（T 0x812dc18）+
//   CharacData::CharacData(this+0x98)（T 0x812d024）。
// [BigP3D 成员化 2026-09-12 判词替换] 旧「字节槽 + placement new」判词（N83 封顶）
//   作废：ORIG 的 C1/D1 都是编译器生成的**成员子对象序列**（含 [except.ctor]
//   cleanup；C1 的 `call ProcStageD1 + _Unwind_Resume` 校验段 / D1 的 pad），
//   4.4-O0 对真实成员 + 初始化列表**逐条同形**（探针 /tmp/bigp3d/probe.cpp 实测：
//   `lea 0x8(%eax),%edx` + `add $0x4,%eax` + `call ProcStageC1(CharacData&)` +
//   `call CharacDataC1` + `jmp` + EH 段 `mov %edx,%ebx; mov %eax,%esi; call
//   ProcStageD1; call _Unwind_Resume`）。初始化列表顺序照 ORIG：m_index 先行，
//   m_procStage(m_characData) 后于声明序执行（成员声明序 = index/procStage/
//   characData），ORIG 传 &m_characData 于 ProcStage ctor 之前构造 CharacData。
//   成员化同时消去 _ZnwjPv/_ZdlPvS_ 两条 placement-new 伪 call（N83）。
StageControl::StageControl()
    : m_index(-1), m_procStage(m_characData), m_characData()
{
}