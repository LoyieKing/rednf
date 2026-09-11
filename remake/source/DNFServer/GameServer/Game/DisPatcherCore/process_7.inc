// ORIG 0x0823349a W _ZN34DisPatcher_ReturnToSelectCharacter7processEP5CUserR8MSG_BASER9ParamBase
// push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，
// 全部逻辑位于 dispatch_sig(0x081fd25c，见 docs/class_func_reports/
// DisPatcher_ReturnToSelectCharacter/dispatch_sig.md；按分工 dispatch_sig 保持 stub)。
int DisPatcher_ReturnToSelectCharacter::process(CUser* /*user*/, MSG_BASE& /*msg*/,

                                                ParamBase& /*param*/)
{
    return 0;
}
