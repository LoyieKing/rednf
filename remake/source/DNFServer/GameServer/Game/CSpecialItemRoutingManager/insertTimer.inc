// ORIG 0x0860b45c _ZN26CSpecialItemRoutingManager11insertTimerEP6CParty
// key=gen_timer_key(0x2b)；InsertTimer(OBJ_TYPE 1, GetPartyIndex(),
// msg 0x2b, sec 0x23, key, 0)。
// b28c-L20: AE — P0 清绑（原 register asm("esi"/"ebx") 违反全域禁令）；P24 内联
// 实参自然形复现 ORIG 的 key→%esi / index→%ebx / G_TimerQueue() 置后三件套
// （ndiff 清绑前 >0 → 0，gap=0，N96 清绑正向例）。
void CSpecialItemRoutingManager::insertTimer(CParty* party)
{
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, party->GetPartyIndex(),
                                (TIMER_MESSAGE)0x2b, 0x23,
                                party->gen_timer_key((TIMER_MESSAGE)0x2b), 0);
}