int Dispatcher_QueryCharacInfo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return LineFunc(0x9f60,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // ORIG 0x08205fc8/0x08205fcf：先 len=0（-0xc）再 name 清零（-0x2a，4.1 内联
    // 对齐剥离慢路；4.4 以 = {0} 表达，形态差归 N78）。
    int len = 0;                                           // -0xc
    char name[0x1e] = {0};                                 // -0x2a
    if (!packet.get_int(len))
    {
        return LineFunc(0x9f65,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(name, 0x1e, len))
    {
        return LineFunc(0x9f66,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (strncmp(name, user->getCurCharacName(), 0x1d) == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8b, 7);
        return 0;
    }
    DB_ReqQueryCharacInfo::makeRequest((int)user->GetUID(), name);
    return 0;
