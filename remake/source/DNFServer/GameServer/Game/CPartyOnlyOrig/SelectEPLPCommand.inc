// ORIG T 0x085a22a8（SelectEPLPCommand.md）：EPLP 选令：置状态 3 并按指定秒数
// 注册 0x14 定时器。
void CParty::SelectEPLPCommand(int cmd)
{
    SetEPLPState(3);
    // b28d-45 P0/N96 清绑：改 P200 内联实参形。
    G_TimerQueue()->InsertTimer(static_cast<TimerEntry::OBJ_TYPE>(1),
                                GetPartyIndex(), static_cast<TIMER_MESSAGE>(0x14),
                                cmd,
                                gen_timer_key(static_cast<TIMER_MESSAGE>(0x14)), 0);
}