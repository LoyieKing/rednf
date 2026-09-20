// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081dd99c orig/ours=42/52 ndiff-blocks=11
//   帧差 sub $28 vs $24 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x28,%esp | mov    0x10(%ebp),%eax | mov    0x14(%ebp),%eax
int Dispatcher_Mercenary_Return::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    if (user == 0)
        return -1;
    if (user->get_state() != 3)
        return -1;

    // [G1Dproc 2026-09-10] ORIG 0x081dd99c（报告 Dispatcher_Mercenary_Return/process.md 全文已读）：
    //   ① 两个引用局部声明在**两道 guard 之后**（ORIG 0x081dd9cb/0x081dd9d1 才写 -0x10/-0xc 槽）；
    //   ② +0x04/+0x05/+0x08 三处写入为**单条带位移 store**
    //      （0x081dd9f9 `mov -0xc(%ebp),%edx; mov %al,0x4(%edx)`、
    //        0x081dda03 `mov -0xc(%ebp),%eax; mov %dl,0x5(%eax)`、
    //        0x081dda0f `mov -0xc(%ebp),%eax; mov %edx,0x8(%eax)`）
    //      ⇒ P0/P82：无具名 `char* pResult` 槽、无 lea 目标地址、无 %ebx 缓存。
    //   ORIG 尾 `mov $0x0,%eax; leave; ret`（我方曾多 push %ebx，随具名指针一并消除）。
    MsgMercenaryReturn& req = reinterpret_cast<MsgMercenaryReturn&>(msg);
    SendMercenaryReturnResult& result = static_cast<SendMercenaryReturnResult&>(param);
    result.m_04 = static_cast<char>(
        user->Return_Mercenary_Charac(req.m_field10, result.m_result));
    result.m_05 = req.m_field0d;
    result.m_08 = req.m_field10;
    return 0;
}