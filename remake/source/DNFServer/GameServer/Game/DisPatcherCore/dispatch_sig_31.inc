int DisPatcher_StartWarRoomTemp::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/)

{
    // ORIG 0x08201b06：纯 LineFunc 占位（报告 DisPatcher_StartWarRoomTemp/dispatch_sig.md）。
    // P31 帧差：ORIG sub $0x28 → dead 局部 0x10。
    volatile char dead_10[0x10]; (void)dead_10;
    return LineFunc(0x9737, __PRETTY_FUNCTION__, 0, 0);
}
