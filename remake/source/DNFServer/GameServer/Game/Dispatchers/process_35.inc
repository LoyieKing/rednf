// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0825fb38 orig/ours=76/77 ndiff-blocks=12
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x14(%ebp),%eax | mov    %eax,-0x24(%ebp) | mov    0x10(%ebp),%eax
int Dispatcher_SecurityLog::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x0825fb38：acc_id = user==0 ? 0 : user->get_acc_id()；
    // 当 *(int*)(msg+0x15)!=0 且 *(int*)(msg+0x19)!=0 时
    // LogManager::logFormat(1, "PacketDispatcher_Impl_2.cpp", PRETTY, 0x9d0,
    //   "Delay check, mode:%d, value:%d, %d, %d, %d, %d, %d, m_id:%s",
    //   *(msg+0xd), *(msg+0x11), *(msg+0x15), *(msg+0x19), *(msg+0x1d), *(msg+0x21),
    //   *(int*)(msg+0x1d) - *(int*)(msg+0x19), NumberToString(acc_id, 0))；
    // 恒返 0。
    // ORIG 0x0825fb41 `mov 0x14(%ebp),%eax; mov %eax,-0x24(%ebp)`（param 引用死槽，+0x24 全函数无读）
    ParamBase& out = param;                                     // -0x24
    (void)out;
    const MSG_SECURITY_LOG_PACKET* m =
        reinterpret_cast<const MSG_SECURITY_LOG_PACKET*>(&msg); // -0x20
    // ORIG 0x0825fb4d `cmpl $0,0xc(%ebp); je +0x28; call; jmp; mov $0,%eax; <L2>: mov %eax,-0x1c`
    // ⇒ 原码三目（两臂落 %eax、汇合单次 store），非 if/else 两臂各自 store。
    unsigned int accId = (user != 0) ? user->get_acc_id() : 0;  // -0x1c
    // ORIG 0x0825fb6e `test; je FAIL`(a==0) + 0x0825fb78 `test; jne LOG`(b!=0)：混合极性。
    // 试过 goto 双守卫形（P223）⇒ 4.4 多发 1 条 jmp（ours 77）；本形（正极性 && + BODY 内
    // return）为 76=76，仅余第二条条件极性/FAIL 岛位置差 [N 类块序]。
    // [P4-P4OrderFix 2026-09-11] 改「否定短路 + 早退 return」：ORIG 为 `test; je FAIL`
    //   (a==0) + `test; jne BODY`(b!=0)，FAIL 岛（`mov $0,%eax; jmp END`）居中、BODY
    //   远端、共享 END。探针 t30 逐条复现该拓扑。语义等价（短路序同、恒返回 0）。
    if (m->m_logCounts[1] == 0 || m->m_logCounts[2] == 0)  // msg+0x15 / +0x19
    {
        return 0;
    }
    LogManager::logFormat(
        1, "PacketDispatcher_Impl_2.cpp",
        __PRETTY_FUNCTION__,
        0x9d0,
        "Delay check, mode:%d, value:%d, %d, %d, %d, %d, %d, m_id:%s",
        m->m_field0d,                                   // msg+0xd
        m->m_logCounts[0],                              // msg+0x11（ORIG 单段直载，无转换中转）
        m->m_logCounts[1],                              // msg+0x15
        m->m_logCounts[2],                              // msg+0x19
        m->m_logCounts[3],                              // msg+0x1d
        m->m_logCounts[4],                              // msg+0x21
        m->m_logCounts[3] - m->m_logCounts[2],
        NumberToString(accId, 0));
    return 0;                                               // ORIG 0x0825fc27 BODY 尾 mov $0,%eax
}