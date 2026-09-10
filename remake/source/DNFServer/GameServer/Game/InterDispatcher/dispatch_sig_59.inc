#pragma pack(push, 1)
// P248 同型化：ORIG 084ccdxx 比较处发 4 行
//   `mov %eax,%edx; mov -0x10(%ebp),%eax; mov 0xe(%eax),%eax; cmp %eax,%edx`
//   ⇒ 字段与 get_charac_no(int) 返回值失配 = unsigned int（探针 t3 实证）。
//   聚合器 Inter_MonitorNoticeRequestMemberEnterToResponserPktView.m_e 为 int
//   （R1 禁改聚合器，故本件自带同布局视图；✅ 建议属主把聚合器该字段改为 unsigned int）。
struct Inter_MonitorNoticeRequestMemberEnterToResponserPktViewI
{
    char m_pad0[0xe];               // +0x00..未消费
    unsigned int m_e;               // +0x0e
    short m_12;                     // +0x12
    char m_14[1];                   // +0x14 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

int Inter_MonitorNoticeRequestMemberEnterToResponser::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    Inter_MonitorNoticeRequestMemberEnterToResponserPktViewI* pPkt = reinterpret_cast<Inter_MonitorNoticeRequestMemberEnterToResponserPktViewI*>(data);
    if (user->get_charac_no(-1) != pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x280c,
            "MEMBER : Inter_MonitorNoticeRequestMemberEnterToResponser::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    if (user->alonePlay_Mode())
    {
        Packet_Monitor_Member_Enter_Reply reply;      // PacketHeader(0x4b9, 0x13)
        reply.m_idByChannel = user->GetUID();     // +0xa（权威头名）
        reply.m_charNo = user->getCurCharacNo();  // +0xe
        reply.m_code = 3;                         // +0x12
        // ORIG 084ccxxx：实参 `lea <reply>,%ebx`（先算）→ 再算 receiver 链
        //   ⇒ 原码是链式单表达式（P0：`T* p = f(); p->g(a);` 无具名 proxy 槽）
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(
            user->GetServerGroup())->SendTcpPacket(reinterpret_cast<char*>(&reply), 0x13);
        return 0;
    }

    if (user->GetGameOptionRef()->get_etc_option((ENUM_OPTION_ETC)0x1d) == 1)
    {
        Packet_Monitor_Member_Enter_Reply reply;      // PacketHeader(0x4b9, 0x13)
        reply.m_idByChannel = user->GetUID();
        reply.m_charNo = user->getCurCharacNo();
        reply.m_code = 4;
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(
            user->GetServerGroup())->SendTcpPacket(reinterpret_cast<char*>(&reply), 0x13);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(0, 0x49);
    packet.put_short((int)pPkt->m_12);
    const int local_10 = (int)strlen(pPkt->m_14);
    packet.put_int(local_10);
    packet.put_str(pPkt->m_14, local_10);
    packet.finalize(true);
    user->Send(packet);
    return 0;
