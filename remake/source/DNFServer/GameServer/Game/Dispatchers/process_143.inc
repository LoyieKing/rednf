int Dispatcher_DeleteToBlackList::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // [b23S3C] ORIG 0x081cdf0a：param 指针首行物化（-0x10 槽，P0 律）；check_error
    // 结果值留 %edx、base 从槽载 %eax 直存 m_4（StartGame@1231 孪生形）；两判定回读
    // p->m_4（UpdateContract@19525 孪生）；mref 入 err==0 段（ORIG +0x7e 才物化 -0xc）；
    // proxy 链式右值 + P48 每用点重取 group（ORIG 两次 GetServerGroup call）。
    stDeleteToBlackListParam* const p = static_cast<stDeleteToBlackListParam*>(&param);
    p->m_4 = check_error(user, msg);
    if (p->m_4 >= 1)
        return 0;
    if (p->m_4 < 0)
        return LineFunc(0x26ea, __PRETTY_FUNCTION__, p->m_4, user->get_acc_id());
    {
        MSG_DELETE_TO_BLACKLIST& mref =
            reinterpret_cast<MSG_DELETE_TO_BLACKLIST&>(msg);   // ORIG -0xc
        Packet_Delete_To_BlackList packet;                // ORIG local_40
        // accId → packet.m_accId(+0x0a)；name → m_name(+0x0e)（ORIG C1 memset 区实测）。
        packet.m_accId = user->get_acc_id();
        strncpy(packet.m_name, mref.m_characName, mref.m_nameLen);
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(
            (ENUM_SERVER_GROUP)user->GetServerGroup())->SendPacket((char*)&packet, 0x2c);
        GlobalData::s_guild_proxy_mgr->GetServerProxy(
            (ENUM_SERVER_GROUP)user->GetServerGroup())->SendPacket((char*)&packet, 0x2c);
    }
    return 0;
