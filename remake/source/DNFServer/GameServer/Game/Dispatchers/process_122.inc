int Dispatcher_MemberEnterReply::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_MEMBER_ENTER_REPLY& mref = reinterpret_cast<MSG_MEMBER_ENTER_REPLY&>(msg);  // 裸偏移→具名字段（同偏移同宽度）
    stMemberEnterReplyParam& out = static_cast<stMemberEnterReplyParam&>(param);
    out.m_4 = check_error(user, msg);
    if (out.m_4 >= 1)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x21ef,
                        __PRETTY_FUNCTION__,
                        out.m_4, user->get_acc_id());
    }
    Packet_Monitor_Member_Enter_Reply reply;   // local_27（栈对象 0x13）
    reply.m_idByChannel = user->GetUID();      // local_1d（+0xa）
    reply.m_charNo = (unsigned int)user->getCurCharacNo();  // local_19（+0xe）
    reply.m_code = mref.m_reply;    // local_15（+0x12）
    CMonitorServerProxy* proxy =
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendTcpPacket(reinterpret_cast<char*>(&reply), 0x13);
    return 0;
