int Inter_MonitorReplyCharacInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_MonitorReplyCharacInfoPktView* pPkt = reinterpret_cast<Inter_MonitorReplyCharacInfoPktView*>(data);
    unsigned short uniqueId = 0;                     // asm -0x12(%ebp) movw $0
    CUser* target = G_CGameManager()->getUser(
        pPkt->m_12, 0xa);  // msg+0x12 characNo
    if (target != 0)
        uniqueId = target->get_unique_id();          // 返回值取 ax

    PacketGuard packet;
    packet.put_header(1, 0x1d6);
    if (pPkt->m_a == 0)    // msg+0x0a 成功分支
    {
        packet.put_byte(1);
        packet.put_byte(                             // msg+0x16（movzbl 零扩展）
            static_cast<int>(pPkt->m_16));
        packet.put_int(static_cast<unsigned int>(uniqueId));
        const int nameLen =                          // msg+0x17 角色名
            static_cast<int>(strlen(pPkt->m_17));
        packet.put_int(nameLen);
        packet.put_str(pPkt->m_17, nameLen);
        packet.put_byte(                             // msg+0x35 short（cwtl 符号扩展）
            static_cast<int>(pPkt->m_35));
        packet.put_byte(                             // msg+0x37 char（movsbl）
            static_cast<int>(pPkt->m_37));
        packet.put_byte(                             // msg+0x38 char（movsbl）
            static_cast<int>(pPkt->m_38));
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(pPkt->m_a);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
