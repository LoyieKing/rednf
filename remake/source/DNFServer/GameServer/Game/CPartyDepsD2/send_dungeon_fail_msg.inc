void CParty::send_dungeon_fail_msg()
{
    int seconds = 10;
    if (static_cast<unsigned short>(m_battleField.m_villageMonsterId) != 0)
        seconds = 3;

    // [b28d-51] P24：key 必须内联为实参（ORIG gen_timer_key 先于 m_partyIndex 装载、
    // 结果留 %ebx 跨 G_TimerQueue call），具名局部引入 -0xc 槽并致帧 0x34→0x30 反。
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, m_partyIndex,
                                (TIMER_MESSAGE)0xe, seconds,
                                gen_timer_key((TIMER_MESSAGE)0xe), 0);
