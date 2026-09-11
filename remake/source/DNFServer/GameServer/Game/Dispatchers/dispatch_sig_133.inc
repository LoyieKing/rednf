int Dispatcher_Security_ProcessScan::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // b28d-D0: P251 无槽多路比较 ⇒ 原码 switch(call 结果)；ORIG 无 -0xc 槽、
    //   帧 sub $0x18（if 链落槽 sub $0x28）。
    switch (_NS_ProcessScan(user, packet))
    {
    case 1:
        return LineFunc(0xde84,
                        __PRETTY_FUNCTION__,
                        0, 0);
    case 2:
        return LineFunc(0xde86,
                        __PRETTY_FUNCTION__,
                        0, 0);
    case 3:
        return LineFunc(0xde88,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}