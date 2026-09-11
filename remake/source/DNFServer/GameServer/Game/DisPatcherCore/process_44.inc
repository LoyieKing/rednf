// ORIG 0x082331bc W _ZN19DisPatcher_UseStack7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制
// 即为空实现（read 同为空）。UseStack 全部业务（读包/校验/应答 0x2f 系列）内嵌于
// dispatch_sig(0x081f63c0，见 docs/class_func_reports/DisPatcher_UseStack/dispatch_sig.md)。
int DisPatcher_UseStack::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }
