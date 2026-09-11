// ============================================================================
// ORIG 0x082335da W _ZN22Dispatcher_ModItemAttr7processEP5CUserR8MSG_BASER9ParamBase
//   objdump 逐指令核实：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret
//   —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x08200b08，本文件上方)
//   与 _SendResult(0x08201938)。忠实转写空返回，勿臆造。
int Dispatcher_ModItemAttr::process(CUser* /*user*/, MSG_BASE& /*msg*/,
                                    ParamBase& /*param*/)
{
    return 0;
}