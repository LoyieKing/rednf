void WarRoom::SendWarRoomFailMsg()
{
    // [b28d-solo-WarRoom · identical] ORIG 0x086c078e | 判词：ORIG 0x086c07a9
    //   `call GenTimerKey; mov %eax,%ebx`（key→%ebx）、0x086c07ae
    //   `mov 0x4(%eax),%esi`（m_index→%esi）、G_TimerQueue 置后 0x086c07b1。
    //   旋钮 = callee 返回类型与实参形参同型：WarRoom.h GenTimerKey 由
    //   unsigned int 改 int（T2 直证：ORIG 调用点无 setcc/无中转，第 5 实参形参
    //   为 int）⇒ 4.4.6 复现同一 callee-saved 分配（探针 /tmp/wr_probe3.cpp
    //   r1=int→key→%ebx/idx→%esi；r2/r5=unsigned→反向）。旧 N7 判词作废。
    //   fresh .o NDIFF=0。
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)3, m_index,
                                (TIMER_MESSAGE)0x46, 10,
                                GenTimerKey((TIMER_MESSAGE)0x46), 0);
