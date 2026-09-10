int Inter_NoticePCRoomPlayerLoginOut::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)                      // asm cmp $0x2; setle
        return 0;

    // ORIG：包基址在 get_state 检查后物化入 -0x10(%ebp)
    Inter_NoticePCRoomPlayerLoginOutPktView* pPkt = reinterpret_cast<Inter_NoticePCRoomPlayerLoginOutPktView*>(data);
    if (user->get_charac_no(-1) !=
        pPkt->m_f)         // msg+0x0f 角色号校验
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x41eb,
            "PCRoom : Inter_NoticePCRoomPlayerLoginOut::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_f);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(0, 0xba);
    packet.put_byte(                                 // msg+0x0a（movsbl 符号扩展）
        static_cast<int>(pPkt->m_a));
    packet.put_byte(                                 // msg+0x13（movzbl 零扩展）
        static_cast<int>(pPkt->m_13));
    const int playerNameLen =                        // msg+0x14 玩家名
        static_cast<int>(strlen(pPkt->m_14));
    packet.put_int(playerNameLen);
    packet.put_str(pPkt->m_14, playerNameLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
