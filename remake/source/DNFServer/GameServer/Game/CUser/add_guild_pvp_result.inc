void CUser::add_guild_pvp_result(int v)
{
    PvP_Room* pRoom = (PvP_Room*)GetPVPRoom();
    if (pRoom == 0)
        return;
    int seat = pRoom->get_user_seat(this);
    if (pRoom->IsPvpObserver(seat) != 0)
        return;
    CEventBase* pEvent = GlobalData::s_event_manager->GetRepeatEvent(0x21);
    if (pEvent != 0 && pEvent->IsEventing((CUser*)0) != 0 &&
        get_charac_guildkey() != 0)
    {
        Packet_Notice_Guild_War_Point_Change notice;
        // 推断：+0 处 guildkey、+4 处点数（ORIG 手动填充后 SendPacket(0xf)）
        *(unsigned int*)((char*)&notice + 0) = (unsigned int)get_charac_guildkey();
        *(char*)((char*)&notice + 4) = (char)v;
        CGuildServerProxy* proxy = GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup());
        if (proxy != 0)
            proxy->SendPacket((char*)&notice, 0xf);
        send_pvp_record();
        PacketGuard packet;
        ((InterfacePacketBuf*)&packet)->put_header(0, 0x6e);
        ((InterfacePacketBuf*)&packet)->put_byte(v);
        ((InterfacePacketBuf*)&packet)->finalize(true);
        Send(packet);
        m_guildWarPoint = m_guildWarPoint + v;
    }
