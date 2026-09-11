// [b28d-30] 复核 DIFF：帧/槽位跨版本差（N82/N93 族），ORIG 0x08633c90 见 report。
// [b28d-solo-TimerDispatcher] 判词 N7 封顶：DIFF→NEAR（95/95 助记符全等）。
// 修法：①P0 删具名 `updateCount`（ORIG 无槽，08633d6c `mov %eax,%ebx` 后 cmp 直用）
// ——内联后 user 槽回 -0x1c（原 -0x20 全帧平移）；②P63 合并短路拆三条独立守卫
// （ORIG 08633cea/08633d09/08633d41 共享同一 leave 出口）；③求值序左操作数写
// GetCurCharacLuckPointUpdateTimeCount（ORIG 先求该 getter 入 %ebx）。
// 残差 = 尾部三 getter callee-saved 角色轮换（ORIG edi/esi/ebx vs 4.4 esi/ebx/edi），
// 实参压栈序相同 ⇒ N7。
// [TimerDispatcher-3 2026-09-10 判据①] 分支目标序列全等；残差 = 尾部三 getter 结果
// edi/esi/ebx 保活序互换（N7）⇒ 无语义缺口。
bool TimerStamina::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 4);   // ORIG 0x08633c90 第2参固定 4（p2 仅用于 loginTick 比对）
    // P63：ORIG 三段独立早退共享同一 leave 出口（08633cea/08633d09/08633d41 均
    // `mov $0,%eax; jmp +0x150`），故拆三条独立守卫而非合并短路。
    if (user == 0)
        return 0;
    if (user->GetLoginTick() != p2)
        return 0;
    if (p3 != 0 && static_cast<unsigned int>(user->get_charac_no(-1)) != p3)
        return 0;
    user->recover_stamina();
    user->IncCurCharacLuckPointUpdateTimeCount();
    // P48 no-CSE：ORIG 每处 use 各自 fresh G_CDataManager()（m_84 / m_80 共 2 次）。
    // P0：ORIG updateCount 无栈槽（0x0b0 `mov %eax,%ebx` 后 cmp 直用）⇒ 原码无具名
    // 局部，表达式内联（旧具名形多占 -0x1c 槽使 user 槽 -0x1c→-0x20 全帧平移）。
    // P243 同族（求值序）：ORIG 先求 GetCurCharacLuckPointUpdateTimeCount（%ebx）后
    // 求 G_CDataManager ⇒ 左操作数写成该 getter（4.4 先求左）。
    if (user->GetCurCharacLuckPointUpdateTimeCount() >=
        G_CDataManager()->m_serverParameter.m_84)
    {
        user->IncCurCharacLuckPoint(G_CDataManager()->m_serverParameter.m_80);
        user->SetCurCharacLuckPointUpdateTimeCount(0);
    }
    // [TD4 2026-09-10 P231/P307 试验·失败] 三 getter 结果改 register 具名局部
    // （按 ORIG 求值序）后四数不变（60/700/4/13）⇒ ORIG 0x08633d6c..0x08633db3 的
    // %edi/%esi/%ebx 角色属 4.1 callee-saved 分配产物（N7），源码侧不可控 ⇒ 回滚内联实参形。
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, user->GetUID(),
                                (TIMER_MESSAGE)2, 0x3c, user->GetLoginTick(),
                                static_cast<unsigned int>(user->getCurCharacNo()));
    return 1;
}