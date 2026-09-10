void CBattle_Field::check_elevator_timer()
{
    ++m_elevatorTimer;

    if (m_elevatorTimer < 4 && m_elevatorState == 2)
    {
        // P24/P52：key→esi、partyIdx→ebx 跨 call 缓存，G_TimerQueue 直通
        // receiver——实参内联形（timerKey/partyIndex 无槽）。
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1,
                                    m_party->GetPartyIndex(),
                                    (TIMER_MESSAGE)0x28, 0xf,
                                    m_party->gen_timer_key((TIMER_MESSAGE)0x28), 0);
    }

    if (m_elevatorState == 2)
        send_elevator_time_packet(0);
