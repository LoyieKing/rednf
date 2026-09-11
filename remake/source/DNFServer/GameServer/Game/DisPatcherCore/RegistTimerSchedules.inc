// ORIG 0x081c7bac T _ZN23DisPatcher_SelectCharac20RegistTimerSchedulesEP5CUser
// （报告 RegistTimerSchedules.md）：G_TimerQueue()->InsertTimer(OBJ_TYPE_0, uid,
// [b28d-29] NEAR=N7/N82附：两 callee-saved 角色对调（ORIG getCurCharacNo->%esi / get_unique_id->%edi，4.4.7 恒反向）；源码已为内联实参形，P248 三档试尽不翻（4.4.6 后端可翻但本波禁改路由）  (ORIG 0x081c7bac insn 34->35)
// TIMER_MESSAGE_0x51, 0, unique_id, characNo) 后恒返 1。
// [b28d-solo-DisPatcherCore] 复核 NEAR：ndiff 8（ORIG 34 / ours 34），残差=N82 槽/块序；N 封顶（详见上方 b28d-29 归因）。
int DisPatcher_SelectCharac::RegistTimerSchedules(CUser* user)

{
    // [b28c-L9] fresh 复验 ndiff 8→4：唯一残差 = 两 callee-saved 角色对调
    // （ORIG getCurCharacNo→%esi / get_unique_id→%edi，4.4 恒 %edi/%esi 反向；
    // 实参装载 `%esi,0x18(%esp)`↔`%edi,0x14(%esp)` 随之互换）
    // ⇒ N82附「多实参 call 首值 callee-saved 寄存器对调」，源形不可择。
    // ORIG 0x081c7bac：实参从右到左求值缓存（getCurCharacNo→esi /
    // get_unique_id→edi / GetUID→ebx），单行嵌套调用复现栈帧（sub $0x2c）。
    // [DPC_CheckErr] 唯一残差 = 三值 callee-saved 分配对调（ORIG 1st→%esi / 3rd→%ebx，
    // 我方 1st→%ebx / 3rd→%esi；中间 u16 值两侧同为 %edi）。
    // [H5DisPatcher 2026-09-11 核账] 去 `static_cast<int>` 试验（拟让 IR 类型与 ORIG
    // 同为 unsigned 以改 callee-saved 取用序）实测 **verdict NEAR→DIFF 劣化**
    // （ours 仍 34）⇒ 已回滚保留 cast 原形，本件记 N18a/N7 封顶。
    G_TimerQueue()->InsertTimer(
        TimerEntry::OBJ_TYPE_0, static_cast<int>(user->GetUID()),
        TIMER_MESSAGE_0x51, 0, user->get_unique_id(),
        user->getCurCharacNo());
    return 1;
}
