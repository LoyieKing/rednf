// ---- UseRevivalItem（报告 UseRevivalItem.md，ORIG T 0x085d1694 sz 0x7d）----
// b28d-solo-ExpertJobOnlyMgr：AE（near 1→0，47 件 identical/AE）。
// ORIG 0x085d1694 多实参 call 首值 callee-saved 角色 = %esi（GetRevivalTime）/
// %ebx（GetUID）；4.4.7 对裸内联实参恒反向（首值 %ebx）。修法 = P231 裸
// `register int` 局部（非 asm 绑定，P0 合规）：两值以 register 提示升格
// callee-saved 且不落槽，分配偏好翻正为 ORIG 的 esi/ebx 序。
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
}