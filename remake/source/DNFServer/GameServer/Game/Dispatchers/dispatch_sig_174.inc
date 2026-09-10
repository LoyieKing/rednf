int Dispatcher_Security_PI_HashModulation_S2C::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // b28d-D0: P251 无槽多路比较 ⇒ 原码 switch(call 结果)；ORIG 无 -0xc 槽、
    //   帧 sub $0x18（if 链落槽 sub $0x28）。
    switch (_NS_PI_2ND_RecvPacketIntegrityModulation_S2C(user, packet))
    {
    case 1:
        return LineFunc(0xe31e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    case 2:
        return LineFunc(0xe320,
                        __PRETTY_FUNCTION__,
                        0, 0);
    case 3:
        return LineFunc(0xe322,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
