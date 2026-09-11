// ============================================================================
// CDispatchServerPacket（拍卖/点券服务器回包分发，ORIG 0x8473660/0x847ac3c/0x847c1fe）
// 语义：从拍卖/点券代理收包，按 packetID 组 SIG 流转投 DISPATCH_Q。
// ============================================================================
// ORIG：dispatch_sig T 0x863a808、RegistNextTimer T 0x863a858、
//   D1/D2 W 0x863e8ce、D0 W 0x863e8fe；无 C1（类从未被构造）。
// 类声明见 TimerDispatcher.h（TimerSig 派生，对象尺寸 4：仅 vptr）。
// dispatch_sig：两代理 IsRunning()==false 时分别补发就绪检查包，恒返 1。
// ---------------------------------------------------------------------------
void Timer_CheckAuctionReady::RegistNextTimer()
{
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                static_cast<TIMER_MESSAGE>(0x98), 0x3c, 0, 0);
}