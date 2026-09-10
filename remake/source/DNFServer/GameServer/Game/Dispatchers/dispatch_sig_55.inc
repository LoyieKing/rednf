int Dispatcher_PvPChannelInfo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0 || user->get_state() < 3) return 0;
    if (user->getCurCharacR() != 0 && user->isDisguiseCharac()) {
        user->setDisguiseCharac(false, '\0', 0);
    }
    if (user->isCompetitionMercenary()) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc6, (unsigned char)0x15);
        return 0;
    }
    if (user->GetSchoolNo() == 0) {
        PacketGuard response;
        response.put_header(1, 0xc6);
        response.put_byte(1);
        response.put_int(0);
        response.put_byte(0);
        response.finalize(true);
        user->Send(response);
        return 0;
    }
    Packet_PvPChannelInfo channelInfo;
    *(unsigned int*)((char*)&channelInfo + 0x0e) = user->GetUID();
    *(unsigned int*)((char*)&channelInfo + 0x0a) = user->getCurCharacNo();
    *(unsigned char*)((char*)&channelInfo + 0x16) = 9;
    *(unsigned int*)((char*)&channelInfo + 0x12) = (unsigned int)user->GetSchoolNo();
    *(unsigned short*)((char*)&channelInfo + 0x02) = 0x18;
    CMonitorServerProxy* proxy =
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendTcpPacket((char*)&channelInfo, 0x18);
    return 0;
