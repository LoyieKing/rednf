int Inter_OnTimeEventRewardStart::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_OnTimeEventRewardStartPktView* pPkt = reinterpret_cast<Inter_OnTimeEventRewardStartPktView*>(data);
    cMyTrace(__PRETTY_FUNCTION__, 0x4fba, 0)(
        "[ON_TIME_EVENT] Reward Start(remain time : %u)",
        pPkt->m_e);

    COnTimeEvent* event =
        reinterpret_cast<COnTimeEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(0x33));
    // [b28d-solo-InterDispatcher] ORIG 0x084e0e2c..0x084e0e4e：两独立守卫同落 else
    // （`cmpl $0x0,-0xc; je` + `call *%edx; test %al,%al; je`）⇒ 短路 && 直判，无 bool 局部。
    // ORIG vtable+0x34 槽 = IsEventing(0)
    if (event != 0 && event->IsEventing(static_cast<CUser*>(0)))
    {
        event->process_set_reward_start(             // ORIG 0x081bbce0（见 NEEDS_DECL）
            pPkt->m_a,     // msg+0x0a ontimeId
            pPkt->m_12,    // msg+0x12 itemNo
            pPkt->m_16,    // msg+0x16 itemCount
            pPkt->m_e);    // msg+0x0e endSpan
        PacketGuard packet;
        packet.put_header(0, 0x105);
        packet.put_int(pPkt->m_e);
        packet.finalize(true);
        G_GameWorld()->send_all_with_state(packet, 3);
    }
    else
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x4fc5, 5)(
            "[ON_TIME_EVENT] isEventing = not, event_no(%u)",
            pPkt->m_a);
    }
    return 0;
