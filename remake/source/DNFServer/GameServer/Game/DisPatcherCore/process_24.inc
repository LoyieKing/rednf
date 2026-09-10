int DisPatcher_SellItem::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232f0e W _ZN19DisPatcher_SellItem7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081f2d0c)（状态检查/Secu 保护/Store::user_sell_item）

// [b28d-29] N6/N22：ORIG 29 条为 (1<<state)&0x15e8 无槽寄存器形；4.4 折叠成 (mask>>state)&1（N6）或槽化（P0），三形实测 33/34 均不达 29  (ORIG 0x081c619c insn 29->34)
