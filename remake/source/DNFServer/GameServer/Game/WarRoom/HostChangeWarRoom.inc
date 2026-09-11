int WarRoom::HostChangeWarRoom(CUser* user, char a, unsigned long b)
{
    // [b28d-solo-WarRoom · identical] ORIG 0x086c08a6 | 判词：①ORIG 0x086c08e4
    //   `lea 0x4(%edx),%ecx` + 0x086c08e7 `mov 0x14(%ebp),%edx` ⇒ m_hostVotes
    //   元素与 b(unsigned long) 同型（WarRoom.h int[6]→unsigned long[6]，Main
    //   代落；探针 wr_probe7 v7/v8 出 lea 形，v1–v3 与 wr_probe8 v14–v19 六形
    //   恒 add）；②GenTimerKey→%ebx / m_index→%esi 同 SendWarRoomFailMsg。
    //   fresh .o NDIFF=0。
    if ((m_slots[(int)a] != 0) && (m_slotsUser[(int)a] == user))
    {
        // [b28BM13 已翻正] ORIG 086c08dd 装载序 a→this→lea→b 由同型化复现。
        m_hostVotes[(int)a] = (int)b;
        if (m_userCount == 0)
        {
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)3, m_index,
                                        (TIMER_MESSAGE)0x47, 5,
                                        GenTimerKey((TIMER_MESSAGE)0x47), 0);
        }
        m_userCount += 1;
    }
    return 0;
}