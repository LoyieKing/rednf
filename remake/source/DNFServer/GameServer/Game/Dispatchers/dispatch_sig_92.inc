int Dispatcher_SetSubGuildMaster::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() < 3 || user->getCurCharacR() == 0) {
        return LineFunc(0x9d97, __PRETTY_FUNCTION__, 0, 0);
    }
    // ORIG 0x082053e6：0x31 连续缓冲（Packet_Monitor_Set_Sub_Guild_Master 头 + 负载数据区）。
    char buffer[0x31];
    Packet_Monitor_Set_Sub_Guild_Master* header = new (buffer) Packet_Monitor_Set_Sub_Guild_Master();
    (void)header;
    // +0x2c 标志字节（ORIG 负载裸偏移，header 仅 PacketHeader 未建字段）。
    // char 对齐 1，packed 视图绑 char& 合规。
    struct __attribute__((packed)) SubGMView
    {
        char m_head[0x2c];
        char m_flag;             // +0x2c
    };
    int& characNo = *reinterpret_cast<int*>(buffer + 0x6);
    int& guildKey = *reinterpret_cast<int*>(buffer + 0xa);
    char* name = buffer + 0xe;
    char& flag = reinterpret_cast<SubGMView*>(buffer)->m_flag;
    characNo = user->get_charac_no(-1);
    guildKey = user->get_charac_guildkey();
    int nameLen = 0;
    if (!packet.get_int(nameLen)) {
        return LineFunc(0x9da1, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_str(name, 0x1e, nameLen)) {
        return LineFunc(0x9da4, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_byte(flag)) {
        return LineFunc(0x9da7, __PRETTY_FUNCTION__, 0, 0);
    }
    if (flag == (char)0x01) {
        return 0;
    }
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendPacket(buffer, 0x31);
    return 0;
