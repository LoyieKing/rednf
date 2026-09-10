void CParty::InsertTournamentDungeonTimer(TIMER_MESSAGE msg)
{
    // [switch+寄存器缓存] ORIG 0x085bd6de：`cmp $0x2e;je; cmp $0x2f;je; jmp`
    // 双 case 共享尾 + key 留 %ebx、m_partyIndex 留 %esi（call 前置装载）。
    switch (msg)
    {
    case (TIMER_MESSAGE)0x2e:
    {
        // [b28d-51] P24：key 内联实参（ORIG key 留 %ebx、m_partyIndex 留 %esi；
        // 具名 key 局部引入 -0x10/-0xc 双槽并使帧 0x20→0x30）。
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, m_partyIndex,
                                    msg, 0x28, (int)gen_timer_key(msg), 0);
        break;
    }
    case (TIMER_MESSAGE)0x2f:
    {
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, m_partyIndex,
                                    msg, 4, (int)gen_timer_key(msg), 0);
        break;
    }
    default:
        break;
    }
