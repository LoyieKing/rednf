int Dispatcher_RequestMemberEnter::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081cb874：param+4 = check_error(user,msg)；>0 返 0；<0 LineFunc(0x2192)；
    // ==0 构造 Packet_Monitor_Request_Member_Enter，UID(+6)/characNo(+0xa)，
    // memcpy(pkt+0xf, msg+0x14, *(size_t*)(msg+0x10))，Monitor 代理 SendTcpPacket(pkt, 0x31)。
    stRequestMemberEnterParam& out = static_cast<stRequestMemberEnterParam&>(param);
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x2192,
                        __PRETTY_FUNCTION__,
                        out.m_4, user->get_acc_id());
    }
    Packet_Monitor_Request_Member_Enter pkt;   // ORIG W ctor 0x0822b9ce（局部帧 -0x40 起）
    // 帧偏移：-0x3a=pkt+0x6(UID)、-0x36=pkt+0xa(characNo)、-0x32=pkt+0xe(30 字节名区)
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&pkt) + 6) = user->GetUID();
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&pkt) + 0xa) = user->getCurCharacNo();
    std::strncpy(reinterpret_cast<char*>(&pkt) + 0xe,
                 reinterpret_cast<const char*>(&msg) + 0x14,
                 *reinterpret_cast<const size_t*>(reinterpret_cast<const char*>(&msg) + 0x10));
    CMonitorServerProxy* proxy =
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendTcpPacket(reinterpret_cast<char*>(&pkt), 0x30);
    return 0;
