void Timer_DungeonStatisticEntrance_Hour::RegistNextTimer()
{
    // ORIG 0x863b9fa：msg=0xab，间隔 3600s
    int interval = 0xe10;   // ORIG -0xc 槽预置（写而不读的 dead local，帧形证据）
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                (TIMER_MESSAGE)0xab, 0xe10, 0, 0);
}