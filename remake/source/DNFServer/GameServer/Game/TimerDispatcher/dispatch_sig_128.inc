bool Timer_FairPvPRematch::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // [b26N7] 尾部虚调用 callee 寄存器 ORIG vfn→%ecx/参数 eax,edx vs 4.4 恒
    // vfn→%eax（N7 探针实证），ndiff 12 封顶。
    PvP_Room* room = G_CGameManager()->GetPvp(p1, (CUser*)0, (int)0x3e);
    if (room == 0)
    {
        return 0;
    }
    if (room->get_timer_key((TIMER_MESSAGE)0x3e) != p2)
    {
        return 0;
    }
    // ORIG +0x61: matcher = *(void**)(room + 0x6e4)（匹配系统指针，ORIG 布局该槽为带虚表对象）
    // ORIG 0x0863cxxx：room 单槽 -0xc，matcher 三次从 +0x6e4 直读（view 链式，
    // 无具名槽）；虚表 +0x64 = IMatch::OnRematchTimeout(room)。
    // ORIG 0x0863a499：正向 `if (matcher != 0){call}`（je 863a4c0 跳 mov$1，
    // null 与 call 后都汇聚 return 1）。
    if (((TimerStartMatchingMatcherSlot*)room)->m_matcher != 0)
        ((TimerStartMatchingMatcherSlot*)room)->m_matcher->OnRematchTimeout(room);
    return 1;
}