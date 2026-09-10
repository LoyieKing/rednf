void CParty::send_loadingcheck_msg()
{
    // [b28d-59] AE（ORIG 0x085b1fe0）：三目 + static volatile const 全局常量读 +
    // key 内联实参（P24）修正后 38/38 逐条一致。
    m_pad119 = 0;   // this+0x119 字节清零（装载检查标志）

    // [b28d-59] ORIG 0x085b1ffd..0x085b200e：cmp $1; jne; mov A,%eax; jmp;
    // mov B,%eax; mov %eax,-0xc ⇒ 三目形 + 全局常量内存读（非 if 赋值形）。
    const int timeout =
        get_member_count() == 1 ? CONST_SINGLE_MAP_LOADING_WAIT_TIMEOUT
                                : CONST_PARTY_MAP_LOADING_WAIT_TIMEOUT;

    // InsertTimer(objType=1, param1=m_partyIndex(+0x64), msg=0xb, sec=timeout,
    //             param2=key, param3=0)
    // [b28d-59] key 内联实参（P24：ORIG 0x085b201f gen_timer_key 先算入 %ebx、
    // m_partyIndex 入 %esi，G_TimerQueue() 置后；具名 key 局部会多栈槽）。
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, m_partyIndex,
                                (TIMER_MESSAGE)0xb, timeout,
                                gen_timer_key((TIMER_MESSAGE)0xb), 0);
