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
