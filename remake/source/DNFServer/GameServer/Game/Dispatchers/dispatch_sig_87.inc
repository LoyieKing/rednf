int Dispatcher_LoadingTimeReport::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0821efec [0x0821efec, 0x0821f611]
    Packet_Loading_Time_Report_Statistics local_824;   // ctor ORIG 0x0822c040（弱内联）
    // 布局无头文件证据，按 ORIG 裸偏移访问（对象栈帧 -0x820 起，payload 尺寸 0x806）。
    // [Lane1Dispatchers 2026-09-10] P183：ORIG **无**具名 base 指针局部——每处访问都
    //   重发 `lea -0x820(%ebp),%eax`（ORIG 0x0821efec 段实证）；我方具名指针占 4B 槽
    //   使整帧 +8、对象落 -0x828（ORIG -0x820）并连带全部槽号错位。改为宏内联
    //   （等价全表达式展开、零具名局部），函数尾 `#undef base`。
