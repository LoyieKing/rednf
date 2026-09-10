int Dispatcher_PvPChannelInfo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 段直证：入口**无** `cmpl $0x0,0xc(%ebp)` user 判空；`cmp $0x2,%eax; setle %al;
    // test %al,%al; je`（4.1 bool 物化，技巧9）⇒ 删 user==0 并改 (bool) 强制物化形。
    if ((bool)(user->get_state() <= 2)) return 0;
    // ORIG 段直证：`call getCurCharacR; test %eax,%eax; setne %al; test %al,%al; je`
    // （4.1 物化；4.4 裸 `!= 0` 折 jne）。
    if ((bool)(user->getCurCharacR() != 0) && user->isDisguiseCharac()) {
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
