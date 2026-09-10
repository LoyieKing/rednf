void TimerGlobalEffectItemTimeout::registTimer(unsigned int type, unsigned int timeoutSec)
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x78, static_cast<int>(timeoutSec),
                                static_cast<int>(type), 0);
