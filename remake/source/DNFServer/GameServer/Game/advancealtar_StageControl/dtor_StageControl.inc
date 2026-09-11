// ----------------------------------------------------------------------------
// ORIG T 0x0812fa06（~StageControl.md 全文）：先 CharacData::~CharacData(this+0x98)
//   再 ProcStage::~ProcStage(this+0x04)（成员逆序析构）。两处子对象指针均直算
//   `mov this,%eax; add $0x98/0x4,%eax; mov %eax,(%esp)`，ORIG 无具名局部（P0：
//   指针全程寄存器/出参槽直传）→ 此处亦内联 reinterpret_cast，不落 -0xc 槽。
//
// [b28d-solo-StageControl] D1：ORIG 29 / ours 13。ORIG（真成员自动 D1）把
//   CharacData::~CharacData(+0x98) 建 try 区，抛时落点 `mov %edx,%ebx; mov %eax,%esi;
//   ProcStage~D1(+0x04); _Unwind_Resume` 后仍走正常路径 ProcStage~D1 ⇒ push esi/ebx +
//   sub 0x10 帧 + 整段 landing pad。我方字节槽 + 显式析构调用无 EH 清理（P67/N91：
//   4.4 对用户显式析构体不建该 pad）。与 C1 同根因（成员化须拆 include 环，见 C1 判词）。
//   可观察语义（逆序销毁 CharacData→ProcStage，指针/调用序/出参槽逐条同 ORIG）等价。
//   N 封顶（P67/N91）。
// [S4AltarStageMisc 2026-09-11 复核] p167：ORIG = CharacData::D2(0x0812fa19) +
//   ProcStage::D2×2（0x0812fa2d EH 段 + 0x0812fa47 正常路径）+ _Unwind_Resume；
//   我方 = CharacData::D1 + ProcStage::D1（D1/D2 同址别名，正常路径逐条等价）。
//   唯一语义差 = 异常路径（CharacData 析构抛异常时 ORIG 仍析构 ProcStage）；4.4 对
//   显式析构体不建 cleanup pad（P67/N91），源码侧无旋钮（cleanup pad 只能由成员
//   子对象/RAII 局部产生，而本处为字节槽显式析构）⇒ N 封顶。
StageControl::~StageControl()
{
    reinterpret_cast<CharacData*>(reinterpret_cast<char*>(this) + 0x98)->~CharacData();
    reinterpret_cast<ProcStage*>(m_procStage)->~ProcStage();
}