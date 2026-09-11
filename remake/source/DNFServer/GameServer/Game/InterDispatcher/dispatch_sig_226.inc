// [b28d-lane2] verdict=DIFF ndiff=107（ORIG 0x084e0dc6）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=67（ORIG 0x84e0dc6）：混合残差（寄存器分配/槽序：N7/N18a/N82/P112）。
// [b28d-SweepInterDispatcher2] verdict=DIFF gap 5→5（120 vs 115，TU 121/1331/5/133 不回退）：真修 cMyTrace 全表达式临时链
//   （P221/P227：`cMyTrace(f,l,v)(fmt, pPkt->m_e)` 使 m_e 在 ctor 前求值并 %ebx 跨 ctor 保活，trace 槽 -0x30/-0x20 与 ORIG 对齐）。
//   残余=N18a×2（process_set_reward_start 的 m_12/m_16 两段升格）+N7 虚调寄存器+else 分支 m_a 未升格。
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
    }
    else
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x4fc5, 5)(
            "[ON_TIME_EVENT] isEventing = not, event_no(%u)",
            pPkt->m_a);
        // P243 块序：与 RewardEnd 同族——ORIG 里发包块在 if/else **之后**，
        //   else 支以 `mov $0x0,%ebx; jmp <函数尾>` 跳过发包 ⇒ 显式 return。
        return 0;
    }
    PacketGuard packet;
    packet.put_header(0, 0x105);
    packet.put_int(pPkt->m_e);
    packet.finalize(true);
    G_GameWorld()->send_all_with_state(packet, 3);
    return 0;
}