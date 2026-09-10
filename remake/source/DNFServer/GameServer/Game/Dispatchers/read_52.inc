int Dispatcher_MailBox_Open::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
// [b28c-31] N（EH 尾块中继）：ORIG 0x081cc3c4 正常路径析构块后为
// `jmp +0xec; nop` 中继（P63 跨版本 EH 尾布局），4.4 恒 fall-through；余量仅此 2 条。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P63] ORIG 0x081cc2e6 orig/ours=72/69 ndiff-blocks=2
//   ORIG 正常路径尾 jmp+nop 中继岛（4.1 EH 尾块布局），4.4 fall-through/jump-thread 恒消除（P223 尾空标签试形实测无效）
//   首差: jmp    081cc3d2 <+0xec> | nop | nop
