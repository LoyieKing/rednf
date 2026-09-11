// ORIG 0x08637e78：InsertTimer(OBJ_TYPE_2, 0, msg=0x77, beginSec, type, 0)。
void TimerGlobalEffectItemBegin::registTimer(unsigned int type, unsigned int beginSec)
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
        static_cast<TIMER_MESSAGE>(0x77), static_cast<int>(beginSec),
        static_cast<int>(type), 0);
}