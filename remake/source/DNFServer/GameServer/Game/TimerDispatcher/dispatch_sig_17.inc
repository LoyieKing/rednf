int TimerClearDungeonCardSelectRight::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x086347e2：GetParty(p1) + check_timer_key(0x13, p2)。
    CParty* party = G_CGameManager()->GetParty(p1);
    // 拆分独立早退（ORIG 0x086347e3：null 段 cmpl;jne 7f0+mov0 岛；
    // check 段 xor$1;test;je 818 前跳 + 811 mov0 岛）。
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x13, p2))
        return 0;
    if (party->get_dungeon_clear_state() == 4)
    {
        party->set_dungeon_clear_state(5);
        party->insert_time_dungeon_clear_msg(5);
    }
    return 1;
