int Dispatcher_Overflow_Info::dispatch_sig(CUser* user, PacketBuf& packet)
{
    Packet_Overflow_Statistic_Add pkt;
    // ORIG 段直证：两个读入目标都经 `lea -0x15(%ebp),%eax; add $0xa/0xb,%eax` 内联
    // 取址（无引用槽）；原具名引用在 -0x18/-0x14 各落一槽，帧 0x24→0x38。
    if (!packet.get_byte(*(unsigned char*)((char*)&pkt + 0xa)))
    {
        return LineFunc(0xd52f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*(unsigned short*)((char*)&pkt + 0xb)))
    {
        return LineFunc(0xd530,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    ENUM_SERVER_GROUP group = user->GetServerGroup();
    CStatisticServerProxy* proxy =
        (*GlobalData::s_statistic_proxy_mgr).GetServerProxy(group);
    proxy->SendPacket((char*)&pkt, 0xd);
    return 0;
