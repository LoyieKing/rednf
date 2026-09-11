// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08206b5c orig/ours=110/119 ndiff-blocks=19
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZZN35Dispatcher_CompleteLoadAfterAssault12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | mov    %eax,%esi | call   080cc1
int Dispatcher_CompleteLoadAfterAssault::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)packet;
    // ORIG 0x08206b5c：完整加载后登记 super power 状态定时器并向全队广播确认。
    if (user->get_state() <= 1)
    {
        return LineFunc(0xa151, __PRETTY_FUNCTION__, 0, 0);
    }
    // [L7 2026-09-11 · 负结论（P321 T3 两端矛盾）] L5 把 CUserCharacInfo.h:964 改为
    //   `unsigned char` 后（定义侧 16→13 = ORIG 形，属该 TU 的真收益），本调用点 110→**113**：
    //   4.4 对 uchar 的布尔语境恒插 `test %al,%al; setne %al; test %al,%al`（ORIG 4.1 为
    //   `test %al,%al; je`）。实测 `if(f())` / `!= 0` / `> 0` **三形皆 113** ⇒ 调用点无法消除；
    //   本件 verdict 不变（DIFF，残余 N7 寄存器差），按「类型轴跨版本连带」归档，保留原写法。
    if (user->GetCurCharacSuperState())
    {
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0,
                                    (int)user->GetUID(),
                                    (TIMER_MESSAGE)0x7f,
                                    G_CDataManager()->m_powerParameter.m_timeAssaultSuperPowerStateAfter,
                                    (int)user->GetLoginTick(),
                                    (unsigned int)user->getCurCharacNo());
    }
    CParty* party = (CParty*)user->GetParty();
    if (party != 0)
    {
        if (party->CheckRecvFromAllMember(user))
        {
            PacketGuard guard;
            guard.clear();
            guard.put_header(1, 0xf8);
            guard.put_byte(1);
            guard.finalize(true);
            party->send_to_party(guard);
            party->SetRecvResultFlag(true);
        }
    }
    return 0;
}