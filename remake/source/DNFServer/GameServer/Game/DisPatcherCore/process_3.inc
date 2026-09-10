int DisPatcher_Exit::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232bea W _ZN15DisPatcher_Exit7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081ed948)

// [b28d-29] N82/setcc-fold：帧 0x40 vs 0x60 + 槽平移与 sete 折痕，调用点全等  (ORIG 0x081c686e insn 267->285)
