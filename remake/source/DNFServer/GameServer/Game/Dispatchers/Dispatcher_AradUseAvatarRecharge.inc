// ==================== Dispatcher_AradUseAvatarRecharge（ORIG 0x819fa56）====================
// dispatch_sig：user==0→0x69c；getCurCharacR==0→0；state∉[3,4]→
//   SendCmdErrorPacket((ENUM_CMDPACKET)0x24d,7)；PVP/FreePvP→同上；CheckInTrade→同上；
//   读 short+short；发内部流 0x30（0x10 字节）。
Dispatcher_AradUseAvatarRecharge::Dispatcher_AradUseAvatarRecharge() throw() {}
