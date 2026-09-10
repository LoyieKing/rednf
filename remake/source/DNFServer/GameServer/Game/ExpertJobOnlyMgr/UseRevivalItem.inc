void CAlchemist::UseRevivalItem(CUser* user)
{
    // b28d-solo 试形：P231 裸 register（非绑定，P0 合规）复现 ORIG 的
    // GetRevivalTime→%esi / GetUID→%ebx 分配偏好（4.4.7 恒反向）。
    user->SetRevivalTime(GlobalData::s_systemTime_.getCurSec() + 0x4b0);
    register int revivalTime = user->GetRevivalTime();
    register int uid = user->GetUID();
    G_TimerQueue()->InsertTimer(
        (TimerEntry::OBJ_TYPE)0, uid, (TIMER_MESSAGE)0x82, 0x4b0,
        revivalTime, 0);
