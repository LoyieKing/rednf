// [b28d-lane2] verdict=DIFF ndiff=160（ORIG 0x084da3e6）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+4。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=164（ORIG 0x84da3e6）：N18a 单段 regmove+P30/N4 add-vs-lea。
int Inter_UpdateServerMessage::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    SIG_UPDATE_SERVER_MESSAGE* msg = reinterpret_cast<SIG_UPDATE_SERVER_MESSAGE*>(data);
    PacketGuard packet;
    // P17/P46：ORIG 084da40a `movzbl %al,%eax`、084da40d `test %eax,%eax`、084da40f `je`、
    //   084da411 `cmp $0x1,%eax`、084da414 `je`、084da41a `jmp` ⇒ 原码是 switch
    //   （不是我方旧稿的 if/else 链）。
    switch (msg->m_kind)
    {
    case 0:
    {   // 本服广播：0xc0 包
        packet.put_header(0, 0xc0);
        packet.put_byte(1);
        packet.put_byte(1);
        packet.put_byte((int)msg->m_messageIndex);
        packet.put_int(msg->m_messageLen);
        packet.put_str(msg->m_message, msg->m_messageLen);
        packet.put_int(msg->m_characNameLen);
        packet.put_str(msg->m_characName, msg->m_characNameLen);
        packet.finalize(true);
        G_GameWorld()->send_all_with_state(packet, 3);
        break;
    }
    case 1:
    {   // 跨服转发：组 monitor 包
        Packet_Monitor_Server_Message_Info info;
        info.m_channel = (char)G_CEnvironment()->get_channel_no();
        info.m_kind = (char)msg->m_kind;
        info.m_index = (char)msg->m_messageIndex;
        info.m_msgLen = msg->m_messageLen;
        info.m_nameLen = msg->m_characNameLen;
        info.m_time = msg->m_updateTime;
        strncpy(info.m_msg, msg->m_message, 0x32);
        strncpy(info.m_name, msg->m_characName, 0x14);

        GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup())
            ->SendTcpPacket((char*)&info, 0x5f);

        // P328⑩/P227：ORIG 0x84da5eb..0x84da65f 先求 8 个实参（time→%edi、name→-0xbc、
        // nameLen→-0xb8、msg→-0xb4、msgLen→-0xb0、index→-0xac、kind→%esi、ch→%ebx）
        // 再 ctor ⇒ 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x42a0, 0)(
            "SERVER_MESSAGE : Send ServerMessage to monitor server(source_channel:%d, kind:%d, index:%d, msg_len:%d, msg:%s, name_len:%d, name:%s, time:%d",
            msg->m_channelNo,
            (int)msg->m_kind,
            (int)msg->m_messageIndex,
            msg->m_messageLen, msg->m_message,
            msg->m_characNameLen, msg->m_characName,
            msg->m_updateTime);
        break;
    }
    default:
        cMyTrace(__PRETTY_FUNCTION__,
            0x42a6, 0)("SERVER_MESSAGE : NOT EXIST SHARE TYPE");
        break;
    }
    return 0;
}