int DisPatcher_SetWarRoomSeatState::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/)
{
    // ORIG 0x082022ca：纯 LineFunc 占位（报告 DisPatcher_SetWarRoomSeatState/dispatch_sig.md）。
    // P31 帧差：ORIG sub $0x38 → dead 局部 0x20。
    volatile char dead_20[0x20]; (void)dead_20;
    return LineFunc(0x9848, __PRETTY_FUNCTION__, 0, 0);
