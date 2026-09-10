int Dispatcher_Overflow_Info::dispatch_sig(CUser* user, PacketBuf& packet)
{
    Packet_Overflow_Statistic_Add pkt;
    unsigned char& isAdd = *(unsigned char*)((char*)&pkt + 0xa);
    unsigned short& cnt = *(unsigned short*)((char*)&pkt + 0xb);
    if (!packet.get_byte(isAdd))
    {
        return LineFunc(0xd52f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(cnt))
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
