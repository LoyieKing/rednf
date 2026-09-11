int Inter_MonitorAddBuddy::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    // ORIG：包基址在 get_state 检查后物化入 -0x10(%ebp)
    Inter_MonitorAddBuddyPktView* pPkt = reinterpret_cast<Inter_MonitorAddBuddyPktView*>(data);
    if (user->getCurCharacNo() != pPkt->m_a)
        return 0;

    PacketGuard packet;
    if (pPkt->m_3a == '\0')
    {   // 成功
        packet.clear();
        packet.put_header(1, 0x3f);
        packet.put_byte(1);
        // ORIG 0x084d15xx：strlen 参走指针算术（add），put_str 参走数组 decay（lea）；
        // len 槽 int 精确匹配 put_str(char*,int)，避免隐式转换重排求值序。
        const int local_10 =
            (int)strlen(reinterpret_cast<char*>(pPkt) + 0x12);
        packet.put_int(local_10);
        packet.put_str(pPkt->m_12, local_10);
        packet.put_byte((int)pPkt->m_30);
        packet.put_byte((int)pPkt->m_32);
        packet.put_byte((int)pPkt->m_33);
        packet.put_int(pPkt->m_34);
        packet.put_byte((int)pPkt->m_38);
        packet.put_byte(
            (unsigned int)pPkt->m_39);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {   // 失败（错误码）
        packet.clear();
        packet.put_header(1, 0x3f);
        packet.put_byte(0);
        packet.put_byte((int)pPkt->m_3a);
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}