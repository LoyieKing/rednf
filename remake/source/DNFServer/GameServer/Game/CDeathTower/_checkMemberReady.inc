void CDeathTower::_checkMemberReady()
{
    // [b28d-solo-CDeathTower] verdict=AE（41/43→归零）：修法 = timeout 由
    // unsigned int 改 int（与 InsertTimer 第 4 参 int 同型，P211 档③），
    // 消 4.4 的 %edi 升格 + push %edi + 帧 0x3c→0x30。
    // 旧残差注记：ORIG volatile const 内存读
    //（0x08c635f0/4）vs 4.4 常量/槽 + push %edi 中转（P0/N 族，既有判词保留）。
    m_playData.resetMemberReady();
    // ORIG 0x84670f8：call 结果直 cmp $0x1（免具名）；key/idx 走 ebx/esi
    // callee-saved（范式2）；函数尾无 mov %eax 置值 ⇒ 返回语义 void。
    // [b28d-D13] N 封顶：ORIG 0x084670f8 timeout@-0xc 直读 volatile const；
    // 4.4 三元恒经 -0x1c 槽 + %edi 中转（帧 0x30 vs 0x3c）。if/else 赋值形实测
    // 12→17 反劣 ⇒ 保持三元。
    // [b28d-solo-CDeathTower] P211 档③：ORIG 0x08467122 三元结果直存 -0xc 槽且
    // 压参时从槽回读；局部与 InsertTimer 第 4 参同型 int（unsigned 局部令 4.4
    // 升格 %edi 并多 push + 帧 0x30→0x3c）。
    int timeout =
        m_party->get_member_count() == 1
            ? CONST_SINGLE_MAP_LOADING_WAIT_TIMEOUT
            : CONST_PARTY_MAP_LOADING_WAIT_TIMEOUT;
    G_TimerQueue()->InsertTimer(
        (TimerEntry::OBJ_TYPE)6, m_idx, (TIMER_MESSAGE)0x49, timeout,
        _genTimerKey((TIMER_MESSAGE)0x49), 0x49);
