int Inter_MonitorCallGuildInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax; sete
        return 0;
    // ORIG：包基址槽在门 1 之后物化。
    Inter_MonitorCallGuildInfoPktView* pPkt = reinterpret_cast<Inter_MonitorCallGuildInfoPktView*>(data);

    // ORIG：同 MonitorCallGuildMembers。
    if (user->get_charac_no(-1) != pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x26e4,
            "Inter_MonitorCallGuildInfo::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_e);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x4d);
    packet.put_byte(1);
    packet.put_byte(
        (int)pPkt->m_12);
    // ORIG 0x84cc10e/0x84cc12f 等 `lea (%edx,%eax,1)` 的 edx = -0x14 具名槽（pPkt）。

// [G2B-Monitor] 残余留 N（写域外）：本 .inc 到上行止，成员环等其余部分由聚合器
//   InterDispatcher.cpp 在 `#include` 之后续写；/tmp/g2_norm 的差异
//   （`imul $0x1d,%eax,%eax; lea (%edx,%eax,1),%eax; add $0x10,%eax; mov 0x3(%eax),%eax`
//   vs 我方 `imul $0x1d,%edx,%edx; add $0x13,%edx; add %edx,%eax; mov (%eax),%eax`，
//   P30/N4 add-vs-lea + 基址折叠族）全部落在续写段 ⇒ 不在本任务写域 ⇒ 留 N。
