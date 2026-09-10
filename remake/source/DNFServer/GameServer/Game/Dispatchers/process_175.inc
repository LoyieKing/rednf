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
    // ORIG 0x081cb8f2: `mov 0x10(%ebp),%eax; mov %eax,-0xc(%ebp)` —— msg 具名引用槽；
    //   0x081cb91f 起长度读 `mov 0x10(%eax),%eax` 常量偏移。
    MSG_BASE& m = msg;
    Packet_Monitor_Request_Member_Enter pkt;   // ORIG W ctor 0x0822b9ce（局部帧 -0x40 起）
    // ORIG 实测帧内偏移（pkt 基 -0x40）：0x081cb90e `mov %eax,-0x36(%ebp)` = pkt+0xa ← GetUID；
    //   0x081cb91c `mov %eax,-0x32(%ebp)` = pkt+0xe ← getCurCharacNo；
    //   0x081cb933 `lea -0x40(%ebp),%eax; add $0x12,%eax` = memcpy/strncpy 目标 pkt+0x12。
    //   （原 +6/+0xa/+0xe 偏移系早期推断，与 ORIG 差 4。）
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&pkt) + 0xa) = user->GetUID();
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&pkt) + 0xe) = user->getCurCharacNo();
    std::strncpy(reinterpret_cast<char*>(&pkt) + 0x12,
                 reinterpret_cast<const char*>(&m) + 0x14,
                 *reinterpret_cast<const size_t*>(reinterpret_cast<const char*>(&m) + 0x10));
    CMonitorServerProxy* proxy =
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendTcpPacket(reinterpret_cast<char*>(&pkt), 0x30);
    return 0;
