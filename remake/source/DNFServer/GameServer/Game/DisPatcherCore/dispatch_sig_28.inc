int DisPatcher_EnterWarRoom::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/)

{
    // ORIG 0x0820229e：纯 LineFunc 占位（报告 DisPatcher_EnterWarRoom/dispatch_sig.md）。
    // P31 帧差：ORIG sub $0x48 vs 最小帧 0x18 → dead 局部 0x30（volatile 保槽零指令）。
    volatile char dead_30[0x30]; (void)dead_30;
    return LineFunc(0x97fb, __PRETTY_FUNCTION__, 0, 0);
}
