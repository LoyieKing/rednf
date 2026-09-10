int Dispatcher_SetSubGuildMaster::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() < 3 || user->getCurCharacR() == 0) {
        return LineFunc(0x9d97, __PRETTY_FUNCTION__, 0, 0);
    }
    // ORIG 0x08205445：`lea -0x3d(%ebp),%eax; mov %eax,(%esp); call ctor`
    //   ⇒ 栈对象直构（无 placement new/operator new/EH）。
    //   偏移直证：get_charac_no 写 -0x33(%ebp)=base+0xa、guildkey 写 -0x2f(%ebp)=base+0xe、
    //   get_str 目标 base+0x12、get_byte 目标 base+0x30（= -0xd(%ebp)），
    //   与 Packet_Monitor_Set_Sub_Guild_Master 具名字段一致（旧形裸偏移整体差 4）。
    //   get_int 目标 -0xc(%ebp) 无预清 ⇒ nameLen 是裸局部（P328⑤）。
    Packet_Monitor_Set_Sub_Guild_Master message;
    message.m_charNo = user->get_charac_no(-1);
    message.m_guildKey = user->get_charac_guildkey();
    int nameLen;
    if (!packet.get_int(nameLen)) {
        return LineFunc(0x9da1, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_str(message.m_name, 0x1e, nameLen)) {
        return LineFunc(0x9da4, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_byte(message.m_grade)) {
        return LineFunc(0x9da7, __PRETTY_FUNCTION__, 0, 0);
    }
    if (message.m_grade == 0x01) {
        return 0;
    }
    // ORIG 0x08205564：GetServerGroup/GetServerProxy 结果全程寄存器直传，
    //   无 proxy 具名局部（P0/P48）。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendPacket(reinterpret_cast<char*>(&message), 0x31);
    return 0;
