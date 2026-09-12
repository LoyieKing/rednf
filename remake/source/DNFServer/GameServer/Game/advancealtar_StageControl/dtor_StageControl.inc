// ----------------------------------------------------------------------------
// ORIG T 0x0812fa06（~StageControl.md 全文）：先 CharacData::~CharacData(this+0x98)
//   再 ProcStage::~ProcStage(this+0x04)（成员逆序析构），CharacData 段带 landing pad
//   （抛时仍析构 ProcStage 后 _Unwind_Resume）。
// [BigP3D 成员化 2026-09-12 判词替换] 旧「显式析构调用 + N 封顶（P67/N91）」作废：
//   pad 并非 lowering 差 —— 它由 [except.ctor] 的**成员销毁序列**语义产生，只有真实
//   成员子对象才触发。改 `~StageControl() {}` 由编译器生成：与 ORIG 0x0812fa06 逐条
//   同形（`sub $0x10` 帧 + push esi/ebx；`mov 0x8(%ebp),%eax; add $0x98,%eax; call
//   CharacDataD1; jmp` 尾块；EH 段 `mov %edx,%ebx; mov %eax,%esi; add $0x4; call
//   ProcStageD1; mov %esi,%eax; mov %ebx,%edx; call _Unwind_Resume`；尾块 `add $0x4;
//   call ProcStageD1`）。空体 ⇒ 无额外指令，全部由 dtor 生成器 + EH 生成器产出。
StageControl::~StageControl()
{
}