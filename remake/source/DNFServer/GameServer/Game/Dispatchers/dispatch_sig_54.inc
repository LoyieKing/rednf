int Dispatcher_Lag_Statistics::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08223de0 [0x08223de0, 0x08224499]
    Packet_Stat_Lag_Statistics local_161;                    // ctor ORIG 0x0822c144（弱内联）
    // [Lane1Dispatchers 2026-09-10] P183：ORIG 无具名 base 指针局部（每处重发
    //   `lea`，0x08223de0 段实证）⇒ 宏内联（零局部），函数尾 `#undef base`。
