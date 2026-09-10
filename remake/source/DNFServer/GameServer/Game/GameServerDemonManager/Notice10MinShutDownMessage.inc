void GameServerDemonManager::Notice10MinShutDownMessage()     // T 0x080f623a
{
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                (TIMER_MESSAGE)0xa2, 0x3c, 0, 0);
