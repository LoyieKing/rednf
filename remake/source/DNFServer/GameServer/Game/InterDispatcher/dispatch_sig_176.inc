int Inter_UpdateServerMessage::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_UpdateServerMessagePktView* pPkt = reinterpret_cast<Inter_UpdateServerMessagePktView*>(data);
    PacketGuard packet;
    // P17/P46：ORIG 084da40a `movzbl %al,%eax`、084da40d `test %eax,%eax`、084da40f `je`、
    //   084da411 `cmp $0x1,%eax`、084da414 `je`、084da41a `jmp` ⇒ 原码是 switch
    //   （不是我方旧稿的 if/else 链）。
    switch (pPkt->m_8)
    {
    case 0:
    {   // 本服广播：0xc0 包
        packet.put_header(0, 0xc0);
        packet.put_byte(1);
        packet.put_byte(1);
        packet.put_byte((int)pPkt->m_9);
        packet.put_int(pPkt->m_msgLen);
        packet.put_str(pPkt->m_10, pPkt->m_msgLen);
        packet.put_int(pPkt->m_nameLen);
        packet.put_str(pPkt->m_48, pPkt->m_nameLen);
        packet.finalize(true);
        G_GameWorld()->send_all_with_state(packet, 3);
        break;
    }
    case 1:
    {   // 跨服转发：组 monitor 包
        Packet_Monitor_Server_Message_Info info;
        info.m_channel = (char)G_CEnvironment()->get_channel_no();
        info.m_kind = (char)pPkt->m_8;
        info.m_index = (char)pPkt->m_9;
        info.m_msgLen = pPkt->m_msgLen;
        info.m_nameLen = pPkt->m_nameLen;
        info.m_time = pPkt->m_time;
        strncpy(info.m_msg, pPkt->m_10, 0x32);
        strncpy(info.m_name, pPkt->m_48, 0x14);

        CMonitorServerProxy* proxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
        proxy->SendTcpPacket((char*)&info, 0x5f);

        // P328⑩/P227：ORIG 0x84da5eb..0x84da65f 先求 8 个实参（time→%edi、name→-0xbc、
        // nameLen→-0xb8、msg→-0xb4、msgLen→-0xb0、index→-0xac、kind→%esi、ch→%ebx）
        // 再 ctor ⇒ 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x42a0, 0)(
            "SERVER_MESSAGE : Send ServerMessage to monitor server(source_channel:%d, kind:%d, index:%d, msg_len:%d, msg:%s, name_len:%d, name:%s, time:%d",
            pPkt->m_4,
            (int)pPkt->m_8,
            (int)pPkt->m_9,
            pPkt->m_msgLen, data + 0x10,
            pPkt->m_nameLen, data + 0x48,
            pPkt->m_time);
        break;
    }
    default:
        cMyTrace(__PRETTY_FUNCTION__,
            0x42a6, 0)("SERVER_MESSAGE : NOT EXIST SHARE TYPE");
        break;
    }
    return 0;
