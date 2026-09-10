#pragma pack(push, 1)
// P248 同型化：ORIG 084ca226~084ca22e 比较处发 4 行
//   `mov %eax,%edx; mov -0x1c(%ebp),%eax; mov 0xe(%eax),%eax; cmp %eax,%edx`
//   ⇒ 字段与 get_charac_no(int) 返回值失配 = unsigned int（c6root 4.4.7 探针 t3：
//   int 字段给 3 行 `mov <slot>,%edx; mov 0xe(%edx),%edx; cmp %edx,%eax`）。
//   聚合器 Inter_MonitorNoticeGuildMarkChangePktView.m_e 为 int（R1 禁改聚合器，
//   故本件自带同布局视图；✅ 建议属主把聚合器该字段改为 unsigned int）。
struct Inter_MonitorNoticeGuildMarkChangePktViewI
{
    char m_pad0[0xa];               // +0x00..未消费
    int m_a;                        // +0x0a
    unsigned int m_e;               // +0x0e
    unsigned int m_12;              // +0x12
};
#pragma pack(pop)

int Inter_MonitorNoticeGuildMarkChange::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0;

    // ORIG 0x084ca1xx：包基址在 state 门之后物化（`jmp` 之后的
    //   `mov 0x10(%ebp),%eax; mov %eax,-0x1c(%ebp)`），声明位置照抄（P0）。
    Inter_MonitorNoticeGuildMarkChangePktViewI* pPkt = reinterpret_cast<Inter_MonitorNoticeGuildMarkChangePktViewI*>(data);


    if (user->get_charac_no(-1) != pPkt->m_e)
    {   // m_uCharID 不匹配 → 日志后退出
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x244a,
            "Inter_MonitorNoticeGuildMarkChange::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    cMyTrace(__PRETTY_FUNCTION__,
        0x244f, 0)("[GUILD MARK CHANGE] Send to client. (channel:%d, character:%u, guildkey:%u)",
          pPkt->m_a,
          pPkt->m_e,
          pPkt->m_12);

    PacketGuard packet;
    packet.put_header(0, 0x3c);
    packet.put_int(pPkt->m_12);
    packet.finalize(true);
    user->Send(packet);
    return 0;
