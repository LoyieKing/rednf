int Inter_TodayGuildMember::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_TodayGuildMemberPktView* pPkt = reinterpret_cast<Inter_TodayGuildMemberPktView*>(data);
    if (user->get_state() <= 2)                        // cmp $2; setle
        return 0;

    // [b28d-lane2] N22 判词推翻（探针 /tmp/l2p/p2.cpp fC）：字段读内联进比较即复现
    // ORIG `mov 0xe(%eax),%ebx; cmp %eax,%ebx`；具名（register）局部必产反向 dst。
    if (pPkt->m_e !=
        user->getCurCharacNo())
        return 0;

    if (pPkt->m_16 != 0 &&
        pPkt->m_16 == user->getCurCharacNo())
    {
        user->SetTodayGuildMember(true);
    }

    PacketGuard packet;
    packet.put_header(0, 0x130);
    if (pPkt->m_16 != 0)
    {
        packet.put_byte(1);
        // len 槽 int 精确匹配 put_str(char*,int)（size_t 会致 decay 参数降级 add+反序）
        const int msgLen = (int)strlen(pPkt->m_1a);
        packet.put_int(msgLen);
        packet.put_str(pPkt->m_1a, msgLen);
        packet.put_byte((int)pPkt->m_38);
        packet.put_byte((int)pPkt->m_39);
        packet.put_byte((int)pPkt->m_3a);
        packet.put_byte((int)pPkt->m_3c);
        packet.put_byte((int)pPkt->m_3b);
    }
    else
    {
        packet.put_byte(0);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
