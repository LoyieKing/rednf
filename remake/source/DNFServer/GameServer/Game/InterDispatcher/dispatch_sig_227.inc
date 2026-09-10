int Inter_OnTimeEventRewardEnd::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    // ORIG 0x084e106a 死写入照抄（P171/P198 两句式）：`mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)`，之后从未读取。
    char* p;
    p = data;
    (void)p;

    cMyTrace(__PRETTY_FUNCTION__, 0x500c, 0)("[ON_TIME_EVENT] Reward End");

    COnTimeEvent* event = reinterpret_cast<COnTimeEvent*>(
        GlobalData::s_event_manager->GetRepeatEvent(0x33));

    // [b28d-solo-InterDispatcher] ORIG 0x084e10be..0x084e10e0：两独立守卫同落 else
    // （`cmpl $0x0,-0xc; je` + `call *%edx; test %al,%al; je`）⇒ 短路 && 直判，无 bool 局部。
    if (event != 0 && event->IsEventing(0))            // vtable+0x34 slot 13
    {
        event->set_reward_state(1);
        if (event->get_ontime_reward_user_count() != 0)
        {
            Timer_Send_Ontime_Reward::RegistNextTimer();
        }
        PacketGuard packet;
        packet.put_header(0, 0x106);
        packet.finalize(true);
        G_GameWorld()->send_all_with_state(packet, 3);
    }
    else
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x501b, 5)(
            "[ON_TIME_EVENT] Inter_OnTimeEventRewardEnd() error");
    }
    return 0;
