// ORIG 0x0823321e W _ZN18DisPatcher_MakePVP7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部建房逻辑（ExtractPacket(0x081f81ba) 解包 MSG_MAKE_PVP_ROOM → CSyncSlangFilter::
// HasSlang 拒绝 0x35/0x9f → MakePVPRoom(cmd=0x35, mode=local_15?6:2)）内嵌于
// dispatch_sig(0x081f803e，报告 docs/class_func_reports/DisPatcher_MakePVP/dispatch_sig.md)。
int DisPatcher_MakePVP::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }
