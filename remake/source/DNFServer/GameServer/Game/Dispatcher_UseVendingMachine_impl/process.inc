// ============================================================================
// ORIG 0x08233dbc W _ZN28Dispatcher_UseVendingMachine7processEP5CUserR8MSG_BASER9ParamBase
//   objdump 逐指令核实：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret
//   —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x0821c2e6，本文件上方)
//   及 6 个 _get/_put/_notify 辅助方法。忠实转写空返回，勿臆造。
int Dispatcher_UseVendingMachine::process(CUser* /*user*/, MSG_BASE& /*msg*/,
                                          ParamBase& /*param*/)
{
    return 0;
}