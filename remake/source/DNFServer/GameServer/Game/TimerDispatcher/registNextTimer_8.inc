// ORIG T 0x086381a0：static 注册链，与其它 registNextTimer 族同构
void TimerConnectMonitorServer::registNextTimer(long sec, int p1, int p2)
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, p1,
                                (TIMER_MESSAGE)0x7a, static_cast<int>(sec),
                                p2, 0);
}