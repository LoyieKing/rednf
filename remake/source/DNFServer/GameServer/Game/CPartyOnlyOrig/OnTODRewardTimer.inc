bool CParty::OnTODRewardTimer(register TIMER_MESSAGE msg)   // CParty.h:592 bool（PSM 落，定义同步）
                                                             // P85：register 形参恢复 ORIG mov;cmp 物化
{
    if (msg == 0x2c)
    {
        _Send_TOD_ClearReward();
        SetEPLPState(2);
        // b28d-45 P0/N96 清绑：`register int key15 asm("esi")` 删除，改 P200
        // 内联实参形（ORIG O@19 `mov%eax,%esi` 由自然升格承载）。
        G_TimerQueue()->InsertTimer(
            static_cast<TimerEntry::OBJ_TYPE>(1), GetPartyIndex(),
            static_cast<TIMER_MESSAGE>(0x15), 0x3c,
            gen_timer_key(static_cast<TIMER_MESSAGE>(0x15)), 0);
        set_state(1);
        G_TimerQueue()->InsertTimer(
            static_cast<TimerEntry::OBJ_TYPE>(1), GetPartyIndex(),
            static_cast<TIMER_MESSAGE>(0x2c), 8,
            gen_timer_key(static_cast<TIMER_MESSAGE>(0x24)), 0x24);
    }
    return 1;
