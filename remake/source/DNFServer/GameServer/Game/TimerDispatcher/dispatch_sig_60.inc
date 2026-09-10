int TimerScheduleGuildWarEnd::dispatch_sig(int p1, int p2, unsigned int /*p3*/)
{
    if (G_GameWorld()->GetChannelType() != 6)
    {
        return 1;
    }
    // ORIG 08633209：CEventManager::GetRepeatEvent(0x21)；vptr+0x34 虚调用判活
    // P0：ORIG 无 event 具名槽（0863320e `mov (%eax),%edx` 直用返回值）⇒ 内联表达式。
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x21)->IsEventing((CUser*)0))
    {
        return 1;
    }
    if (p2 == 10)
    {
        PacketGuard packet;  // ORIG local @ -0x14(%ebp)
        packet.put_header(0, 0x6d);
        packet.put_byte(10);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        // ORIG 086332d5：InsertTimer(OBJ_TYPE_2,0,0x62,300,5,0)
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x62, 300, 5, 0);
    }
    else if (p2 == 5)
    {
        PacketGuard packet;  // ORIG local @ -0x20(%ebp)
        packet.put_header(0, 0x6d);
        packet.put_byte(5);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        // ORIG 086333a9：InsertTimer(OBJ_TYPE_2,0,0x62,60,4,0)
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x62, 60, 4, 0);
    }
    else if (p2 == 4)
    {
        PacketGuard packet;  // ORIG local @ -0x2c(%ebp)
        packet.put_header(0, 0x6d);
        packet.put_byte(4);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        // ORIG 0863347d：InsertTimer(OBJ_TYPE_2,0,0x62,60,3,0)
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x62, 60, 3, 0);
    }
    else if (p2 == 3)
    {
        PacketGuard packet;  // ORIG local @ -0x38(%ebp)
        packet.put_header(0, 0x6d);
        packet.put_byte(3);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        // ORIG 08633551：InsertTimer(OBJ_TYPE_2,0,0x62,60,2,0)
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x62, 60, 2, 0);
    }
    else if (p2 == 2)
    {
        PacketGuard packet;  // ORIG local @ -0x44(%ebp)
        packet.put_header(0, 0x6d);
        packet.put_byte(2);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        // ORIG 08633625：InsertTimer(OBJ_TYPE_2,0,0x62,60,1,0)
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x62, 60, 1, 0);
    }
    else if (p2 == 1)
    {
        PacketGuard packet;  // ORIG local @ -0x50(%ebp)
        packet.put_header(0, 0x6d);
        packet.put_byte(1);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        // ORIG 086336f9：InsertTimer(OBJ_TYPE_2,0,0x62,60,0,0)
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x62, 60, 0, 0);
    }
    else if (p2 == 0)
    {
        // ORIG 0863373e：TriggerEventEnd(0x21)
        GlobalData::s_event_manager->TriggerEventEnd(0x21);
    }
    return 1;
