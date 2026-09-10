int Inter_MonitorNoticeMemberEnterOk::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0;

    // ORIG：包基址在 get_state 检查后物化入 -0x18(%ebp)
    Inter_MonitorNoticeMemberEnterOkPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeMemberEnterOkPktView*>(data);
    if (user->get_charac_no(-1) != pPkt->m_11)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x288b,
            "MEMBER : Inter_MonitorNoticeMemberEnterOk::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_11);
        return 0;
    }

    if (pPkt->m_result == 2)
    {   // 进入确认 → 0x4a 状态包
        PacketGuard packet;
        packet.put_header(0, 0x4a);
        packet.put_byte((int)pPkt->m_result);
        packet.put_byte((int)pPkt->m_sub);
        packet.put_byte((int)pPkt->m_15);
        const int nameLen = strlen(pPkt->m_16);
        packet.put_int(nameLen);
        packet.put_str(pPkt->m_16, nameLen);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }
    else if (pPkt->m_result == 3)
    {
        user->Send_SoloPlay_Reject_NotiPack(
            reinterpret_cast<char*>(pPkt) + 0x16,      // ORIG 经 -0x18 槽
            static_cast<ENUM_SOLO_PLAY_REJECT_MODE>(1),
            static_cast<ENUM_SOLO_PLAY_REJECT_MSG>(6));
        return 0;
    }
    else if (pPkt->m_result == 4)
    {
        user->Send_SoloPlay_Reject_NotiPack(
            reinterpret_cast<char*>(pPkt) + 0x16,      // ORIG 经 -0x18 槽
            static_cast<ENUM_SOLO_PLAY_REJECT_MODE>(0),
            static_cast<ENUM_SOLO_PLAY_REJECT_MSG>(6));
        return 0;
    }

    if (pPkt->m_sub == 1 &&
        pPkt->m_flag == 1)
    {   // 上线会员标记：设置连接会员/经验等级并刷新队伍信息
        user->set_connect_upper_member(pPkt->m_uint);
        user->set_uppermember_exp_level(
            pPkt->m_34);
        CParty* party = (CParty*)user->GetParty();   // CUser.h 冻结：GetParty 返回 void*
        if (party != 0)
            party->send_party_realtime_info();
    }
    user->set_charac_memberkey(pPkt->m_11);

    PacketGuard packet;
    packet.put_header(0, 0x4a);
    packet.put_byte((int)pPkt->m_result);
    packet.put_byte((int)pPkt->m_sub);
    packet.put_byte((int)pPkt->m_15);
    const int nameLen = strlen(pPkt->m_16);
    packet.put_int(nameLen);
    packet.put_str(pPkt->m_16, nameLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
