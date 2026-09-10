int Dispatcher_ApproveJoinGuild::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_APPROVE_JOIN_GUILD& mref = reinterpret_cast<MSG_APPROVE_JOIN_GUILD&>(msg);  // 裸偏移→具名字段（同偏移同宽度）
    if (user->get_state() != 3)
    {
        return 0;
    }
    if (!user->IsGuildMaster() && !user->IsSubGuildMaster())
    {
        return 0;
    }
    Packet_Approve_Join_Guild packet;   // local_26（ORIG 栈对象 0x16；头仅导出 ctor，字段经裸偏移）
    *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(&packet) + 0x0a) =
        (unsigned int)user->get_charac_guildkey();                      // +0xa guildKey
    *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(&packet) + 0x0e) =
        (unsigned int)user->getCurCharacNo();                           // +0xe charNo
    *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(&packet) + 0x12) =
        mref.m_characNo;                            // +0x12 no
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendTcpPacket(reinterpret_cast<char*>(&packet), (unsigned int)0x16);
    return 0;
