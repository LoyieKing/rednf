void SendUserInfoToUpperServer(Packet_Monitor_UDP_Reply_UserInfo& pkt,
                               unsigned char type, CUser*, 
                               ENUM_SERVER_GROUP group)
{
    if (type == 0xcb)
    {
        // b28d-lane12: P52——ORIG 0x0829611a 先把 pkt.packetSize/&pkt 物化到
        // %esi/%ebx 并跨 GetServerProxy 调用保活，返回值直传 SendTcpPacket 的 this
        // ⇒ 单条链式表达式（具名 proxy 局部会坍缩为栈槽形）。
        GlobalData::s_guild_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)(int)group)
            ->SendTcpPacket((char*)&pkt, pkt.packetSize);  // ORIG *(u16*)(pkt+2)
    }
    else if (type == 0xc9)
    {
        GlobalData::s_monitor_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)(int)group)
            ->SendTcpPacket((char*)&pkt, pkt.packetSize);
    }
    memset(&pkt.m_items, 0, 0x16da);  // ORIG memset(pkt+0xc,0,0x16da)
