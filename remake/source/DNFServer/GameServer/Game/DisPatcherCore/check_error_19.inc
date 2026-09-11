// ---------------------------------------------------------------------------
// ORIG W 0x08595a6c _ZN21DisPatcher_FinishLoad{C1,C2}Ev —— 无独立报告（该类报告目录
// 仅 process/check_error/Party_hack_check/read），objdump 0x8595a6c 转写：
// call 模板基类实例 C1（IPacketDispatcher<MSG_FINISH_LOADING,stFinishLoadParam,
// ch_state0>::C1，ORIG 0x85984c6；PacketTypes.cpp 显式实例化）后写最终虚表指针。
// ---------------------------------------------------------------------------
// ORIG T 0x08264bfe _ZN21DisPatcher_FinishLoad11check_errorEP5CUserR8MSG_BASE
// （docs/class_func_reports/DisPatcher_FinishLoad/check_error.md ## 汇编 + ## 反编译 C
// 全文转写）。ORIG mov 0xc(%ebp) 在隐式 this(+0x8) 之后即第一具名参数 user——
// Ghidra 伪 C 的 "GetDebugCommand((CUser*)param_2)" 为其省略 this 的编号假象，
// 实参就是 user。GetDebugCommand()==1 → 0x7fffffff，否则 0；
// if/else 形态对应 ORIG sete/test/je 分支骨架。
// [b28d-solo-DisPatcherCore] P211 callee 侧旋钮（同 SetPartyInfo）。
unsigned int DisPatcher_FinishLoad::check_error(CUser* user, MSG_BASE& /*msg*/)

{
    if (user->GetDebugCommand() == 1)
    {
        return 0x7fffffff;
    }
    return 0;
}
