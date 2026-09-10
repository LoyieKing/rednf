#pragma pack(push, 1)
// P248 同型化：ORIG 084c92xx 比较处发 4 行
//   `mov %eax,%edx; mov -0x10(%ebp),%eax; mov 0xe(%eax),%eax; cmp %eax,%edx`
//   ⇒ 字段与 get_charac_no(int) 返回值失配 = unsigned int（探针 t3 实证，同 38/39/40）。
//   聚合器 Inter_MonitorNoticeGuildEnterPktView.m_e 为 int（R1 禁改聚合器，
//   故本件自带同布局视图；✅ 建议属主把聚合器该字段改为 unsigned int）。
struct Inter_MonitorNoticeGuildEnterPktViewI
{
    char m_pad0[0xe];               // +0x00..未消费
    unsigned int m_e;               // +0x0e
    unsigned short m_12;            // +0x12
    int m_14;                       // +0x14
    char m_pad18[0x4];              // 未消费
    unsigned int m_1c;              // +0x1c
    char m_20[1];                   // +0x20 串/字节区首元素（容量未建模）
    char m_pad21[0x16];             // 未消费
    char m_37[1];                   // +0x37 串/字节区首元素（容量未建模）
    char m_pad38[0x1d];             // 未消费
    // ORIG 084c9259 段 `mov 0x55(%eax),%ebx`（单段直读，无中转）＝ 与
    //   CUserCharacInfo::getCurCharacNo()（unsigned int，CUserCharacInfo.h:783）**同型**
    //   ⇒ 本字段 unsigned int（我方旧 int 会多一条 `mov %eax,%ebx` 中转，P211 档）。
    unsigned int m_55;              // +0x55
    unsigned char m_59;             // +0x59
};
#pragma pack(pop)

int Inter_MonitorNoticeGuildEnter::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    Inter_MonitorNoticeGuildEnterPktViewI* pPkt = reinterpret_cast<Inter_MonitorNoticeGuildEnterPktViewI*>(data);
    if (user->get_charac_no(-1) != pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2305,
            "Inter_MonitorNoticeGuildEnter::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    if (pPkt->m_55 == user->getCurCharacNo())
    {
        // [b28d-solo-InterDispatcher] P0：ORIG 0x084c9259..0x084c92b4 中 cVar1→%ebx、
        // uVar2→%edi、uVar4→%esi 均无栈槽（内联右值）；仅 pcVar8 落 -0x3c（具名局部）。
        char* pcVar8 = data + 0x20;
        user->m_historyLog.GuildJoin(
            user->get_charac_guildkey(), pcVar8, pPkt->m_1c, pPkt->m_12,
            pPkt->m_59 != 0 ? '\x01' : '\0');
    }

    PacketGuard packet;
    packet.put_header(0, 0x3a);
    packet.put_int(pPkt->m_14);
    packet.put_int(pPkt->m_1c);
    const int local_24 = (int)strlen(pPkt->m_20);
    packet.put_int(local_24);
    packet.put_str(pPkt->m_20, local_24);
    const int local_20 = (int)strlen(pPkt->m_37);
    packet.put_int(local_20);
    packet.put_str(pPkt->m_37, local_20);
    packet.finalize(true);
    user->Send(packet);
    return 0;
