// ==================== Dispatcher_AradAvatarConvert（ORIG 0x81a00be）====================
// dispatch_sig：user==0→0x74a；getCurCharacR==0→0；state∉[3,4]→0；PVP→
//   SendCmdErrorPacket((ENUM_CMDPACKET)0x24f,7)；CheckInTrade→同上；isEnableAvatarSocketAction
//   （未建模，推断 0）；读 short+short+int；发内部流 0x33（0xe 字节）。
Dispatcher_AradAvatarConvert::Dispatcher_AradAvatarConvert() throw() {}
