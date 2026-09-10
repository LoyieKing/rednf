int DisPatcher_SetIP::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232fd2 W _ZN16DisPatcher_SetIP7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081f468c)

// [b28d-29] N78/N82：ORIG 4 个 dword 清零 vs 4.4 逐字节 movb 展开 + 帧 0x60 vs 0xa0；语义等价  (ORIG 0x081ed948 insn 316->373)
