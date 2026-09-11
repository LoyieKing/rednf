int Dispatcher_Security_MemoryIntegrity::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // b28d-D0: P251 无槽多路比较 ⇒ 原码 switch(call 结果)；ORIG 无 -0xc 槽、
    //   帧 sub $0x18（if 链落槽 sub $0x28）。
    switch (_NS_MI_RecvMemoryIntegrityModulation(user, packet))
    {
    case 1:
        return LineFunc(0xdf95,
                        __PRETTY_FUNCTION__,
                        0, 0);
    case 2:
        return LineFunc(0xdf97,
                        __PRETTY_FUNCTION__,
                        0, 0);
    case 3:
        return LineFunc(0xdf99,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}