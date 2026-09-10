int Inter_ChangeGuildName::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    char* recv = data;                                 // -0x10(%ebp) 缓存消息指针

    PacketGuard packet;
    packet.put_header(1, 0xb6);
    if (reinterpret_cast<Inter_ChangeGuildNamePktView*>(recv)->m_16 == 0)
    {   // 成功 → 本地改名并同步 proxy
        packet.put_byte(1);
        const int nameLen = strlen(reinterpret_cast<Inter_ChangeGuildNamePktView*>(recv)->m_17);
        packet.put_int(nameLen);
        packet.put_str(reinterpret_cast<Inter_ChangeGuildNamePktView*>(recv)->m_17, nameLen);
        user->ChangeGuildName(recv + 0x17);
        // [b28d-lane2] ORIG 084d50f6 段：GetServerProxy 结果直落 (%esp)（无具名 proxy 槽，
        // P183/P0）——旧具名 proxy 占 4B 使全帧槽号深移 4。
        GlobalData::s_guild_proxy_mgr->GetServerProxy(
            user->GetServerGroup())->ChangeGuildNameInProxy(
                reinterpret_cast<Inter_ChangeGuildNamePktView*>(recv)->m_e,
                recv + 0x17);
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte((unsigned int)reinterpret_cast<Inter_ChangeGuildNamePktView*>(recv)->m_16);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
