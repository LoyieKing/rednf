// ---- pvp_assault 超时定时器注册（ORIG T，四函数同构，仅 TIMER_MESSAGE 不同）----
// objdump 0x863755a/0x86375ca/0x863763a/0x86376aa：
//   G_TimerQueue()->InsertTimer(OBJ_TYPE_2, p1, msg, (int)sec, p2, 0)
void TimerCompleteLoadAssaultTimeout::registNextTimer(long sec, int p1, int p2)
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, p1,
                                (TIMER_MESSAGE)0x6e, static_cast<int>(sec),
                                p2, 0);
}