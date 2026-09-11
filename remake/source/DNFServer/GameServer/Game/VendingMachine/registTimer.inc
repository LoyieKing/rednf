// ----------------------------------------------------------------------------
// TimerVendingMachineItemNotify::registTimer（ORIG 0x08638074）—— static
// ----------------------------------------------------------------------------
void TimerVendingMachineItemNotify::registTimer(int uid, unsigned long itemIdx,
                                                unsigned int count,
                                                unsigned int type)
{
    // ORIG：G_TimerQueue()->InsertTimer(OBJ_TYPE_0, param_1, TIMER_MESSAGE(0xa),
    //   sec=param_4, param2=param_2, param3=param_3)。
    // P24：实参内联——itemIdx/count 物化 callee-saved 跨 G_TimerQueue call。
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, uid, (TIMER_MESSAGE)0xa,
                                (int)type, (int)itemIdx, count);
}