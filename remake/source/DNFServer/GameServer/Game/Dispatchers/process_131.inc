int Dispatcher_RequestBlackList::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081ce070：check_error 结果写 param+0x04；
    // err>0 → 0；err<0 → LineFunc(0x2717)；err==0 → 向 monitor 请求黑名单。
    stRequestBlackListParam& out = static_cast<stRequestBlackListParam&>(param);
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        LineFunc(0x2717,
                 __PRETTY_FUNCTION__,
                 out.m_4, (unsigned int)user->get_acc_id());
        return 0;
    }

    // ORIG 0x0822bbb2 ctor；pkt+0x0a = get_acc_id(0x080da36e)；
    // pkt+0x0e = GetUID(0x080c8c96)；SendPacket(pkt, 0x12)。
    Packet_Request_BlackList pkt;
    *(unsigned int*)(reinterpret_cast<char*>(&pkt) + 0x0a) = user->get_acc_id();
    *(unsigned int*)(reinterpret_cast<char*>(&pkt) + 0x0e) = user->GetUID();

    ENUM_SERVER_GROUP group = user->GetServerGroup();
    CMonitorServerProxy* proxy = GlobalData::s_monitor_proxy_mgr->GetServerProxy(group);
    proxy->SendPacket(reinterpret_cast<char*>(&pkt), 0x12);
    return 0;
