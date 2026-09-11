// ORIG T 0x08637ef6 _ZN28TimerGlobalEffectItemTimeout11registTimerEjj
void TimerGlobalEffectItemTimeout::registTimer(unsigned int type, unsigned int timeoutSec)
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x78, static_cast<int>(timeoutSec),
                                static_cast<int>(type), 0);
}