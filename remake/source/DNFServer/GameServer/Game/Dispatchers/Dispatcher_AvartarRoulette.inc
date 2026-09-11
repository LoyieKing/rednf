// ==================== Dispatcher_AvartarRoulette（ORIG 0x819ef54）====================
// dispatch_sig：user==0→0x312；getCurCharacR==0→0；state<3→SendCmdErrorPacket
//   (0x24a,7) 返回 0；CheckInTrade→同上；PVP/FreePvP→同上；读 2 字节
//   （slotType<0xb、count<10）；通过后发内部流 0x2c（3 字节）。
//   checkWorkState 安全检查依赖未建模服务（ServiceRestrictManager 等），推断为通过。
Dispatcher_AvartarRoulette::Dispatcher_AvartarRoulette() throw() {}
