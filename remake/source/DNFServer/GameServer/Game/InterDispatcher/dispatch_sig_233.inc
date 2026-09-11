// [b28d-solo-InterDispatcher 收口] verdict=AE（P231 register int 局部直落 %ebx → 归零）。
int Inter_NoticePvPBuddyInOut::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_NoticePvPBuddyInOutPktView* pPkt = reinterpret_cast<Inter_NoticePvPBuddyInOutPktView*>(data);
    if (user->get_state() <= 2)                        // setle：signed <= 2
        return 0;

    register int characNoMsg = pPkt->m_a;
    if (characNoMsg != user->getCurCharacNo())
        return 0;

    PacketGuard packet;
    if (pPkt->m_12 == 0)
    {   // 登出（0x10d）附结果码
        packet.put_header(0, 0x10d);
        packet.put_byte((unsigned int)pPkt->m_13);
    }
    else
    {   // 登入（0x10e）
        packet.put_header(0, 0x10e);
    }
    packet.put_byte((int)pPkt->m_14);
    const int nameLen = strlen(pPkt->m_15);
    packet.put_int(nameLen);
    packet.put_str(pPkt->m_15, nameLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}