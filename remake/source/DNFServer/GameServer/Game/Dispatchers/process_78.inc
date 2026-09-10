int Dispatcher_CallMemberList::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081cbda2：check_error 结果写 param+0x04；
    // err>0 → 0；err<0 → LineFunc(0x225b)；err==0 → 向 monitor 请求成员列表。
    stCallMemberListParam& out = static_cast<stCallMemberListParam&>(param);
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        LineFunc(0x225b,
                 __PRETTY_FUNCTION__,
                 out.m_4, (unsigned int)user->get_acc_id());
        return 0;
    }

    // ORIG 0x0822ba76 ctor；pkt+0x0a = GetUID(0x080c8c96)；
    // pkt+0x0e = getCurCharacNo(0x080cbc4e)；SendPacket(pkt, 0x12)。
    Packet_Monitor_Call_Member_List pkt;
    *(unsigned int*)(reinterpret_cast<char*>(&pkt) + 0x0a) = user->GetUID();
    *(int*)(reinterpret_cast<char*>(&pkt) + 0x0e) = user->getCurCharacNo();

    ENUM_SERVER_GROUP group = user->GetServerGroup();
    CMonitorServerProxy* proxy = GlobalData::s_monitor_proxy_mgr->GetServerProxy(group);
    proxy->SendPacket(reinterpret_cast<char*>(&pkt), 0x12);
    return 0;
