// ORIG 0x084e1062 T dispatch_sig —— 准时事件奖励结束：置奖励态并全员广播结束包。
// 报告 docs/class_func_reports/Inter_OnTimeEventRewardEnd/dispatch_sig.md 全文转写。
// [b28d-lane2] verdict=DIFF ndiff=96（ORIG 0x084e1062）：寄存器中转(N18a/N7)+混合残差；gap+1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=53（ORIG 0x84e1062）：N82/P112 帧槽平移+P30/N4 add-vs-lea。
// [b28d-SweepInterDispatcher2] verdict=DIFF gap 5→3（102 vs 99，TU 121/1331/5/133 不回退）：真修两处——
//   ① 补 ORIG 0x084e106a 死写入 `char* p; p=data;`（P171/P198 两句式，ORIG `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)` 后从未读）；
//   ② 两个 cMyTrace 改全表达式临时链（P221/P227，消 else 分支具名对象槽）。残余=N7 虚调寄存器+块序（N104）。
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
    }
    else
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x501b, 5)(
            "[ON_TIME_EVENT] Inter_OnTimeEventRewardEnd() error");
        // P243 块序：ORIG 0x084e115b `mov $0x0,%ebx; jmp 084e11e2<+0x180>`（函数尾）
        //   ⇒ else 支显式 return，跳过下方发包块；而发包块在 ORIG 里位于 if/else **之后**
        //   （0x084e10bc `jmp 084e1160<+0xfe>` 越过 else 块 0xbe..0xfd）。
        return 0;
    }
    PacketGuard packet;
    packet.put_header(0, 0x106);
    packet.finalize(true);
    G_GameWorld()->send_all_with_state(packet, 3);
    return 0;
}