int Dispatcher_SecuDataControl::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0821f612：byte + 两个 short 逐段读取；任一失败走对应行号 LineFunc；
    // 全部成功 → GlobalData::s_pSecuDataControl->SetControlData(user, b, s1, s2)。
    unsigned char b = 0;
    unsigned short s1 = 0;
    unsigned short s2 = 0;

    if (!packet.get_byte(b))
    {
        return LineFunc(0xd838,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(s1))
    {
        return LineFunc(0xd839,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(s2))
    {
        return LineFunc(0xd83a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    GlobalData::s_pSecuDataControl->SetControlData(user, b, s1, s2);
    return 0;
}