// ORIG T 0x08130644（onEplp.md 全文）：sendEplp(endType) +
//   Timer_StageControl::RegistTimerEPLPReturnVillage(getIndex())
//   （ORIG T 0x0812f956，定义 advancealtar_ProcStage.cpp）。
// ----------------------------------------------------------------------------
// [b28d-solo-StageControl] onEplp：ORIG 27 / ours 29。
// [round5-A29 2026-09-10 破 N] 真因不是「P85 跨语句 %eax 保活不可造」——ORIG 0x08130644 的
//   `mov 0xc(%ebp),%eax; test %eax,%eax; js END; cmp $0x2,%eax; jle ZERO; cmp $0x3,%eax;
//   je ONE; jmp END` 是 **4.1 对 switch 的决策树**（P163/P324/P286），不是 if 链产物：
//   if 链在 4.4 恒按语句重载形参并折 `cmpl $3,0xc(%ebp)` 内存直比（P85），而 switch 形
//   4.4 同样产「单次载入 + 寄存器比较链」（P286 单 case switch 族）。探针
//   /tmp/dtprobe/flagprobe2.cpp `f5` 与 ORIG 逐条同形（movb 预清 → movl 形参 → testl →
//   js → cmpl $2 → jle → cmpl $3 → je → jmp → movb $0 → jmp → movb $1）。
//   叶块物理序 = case 书写序（P324）：case 0/1/2 的 ZERO 块在 case 3 的 ONE 块之前。
//   StageEndType::T 值域 END_N0..END_N3 含 0，无 P305 越界删 case 风险；
//   `(int)` 显式转换即 f5 的 int 形参口径（同宽零开销）。
void StageControl::onEplp(StageEndType::T endType)
{
    // ORIG 0x813064a：bool 岛 -0x9（预清 movb $0x0、case 叶块物化 movb $0/$1、无读者——
    // 4.1 保留死 store；volatile 强制 4.4 保留写与槽）。
    volatile bool flag = false;
    switch ((int)endType)
    {
    case 0:
    case 1:
    case 2:
        flag = false;
        break;
    case 3:
        flag = true;
        break;
    }
    sendEplp(endType);
    Timer_StageControl::RegistTimerEPLPReturnVillage(getIndex());
}