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
    struct MercView          // MsgMercenaryReturn 视图：m_field10@+0x10（0xd+1+2=16 已 4 对齐）
    {
        char m_head[0xd];
        char m_field0d;          // +0x0d byte
        char m_pad0e[2];         // +0x0e..+0x0f
        unsigned int m_field10;  // +0x10
    };
    struct SendResultView    // SendMercenaryReturnResult 视图（pack(1)，+0x0c 起为 m_result）
    {
        int m_00;
        char m_04;
        char m_05;
        char m_pad06[2];
        unsigned int m_08;
    };
    // 实参右→左（ORIG 0x081dd9d7 先 `lea 0xc(%eax),%edx` 取 m_result、再读 msg+0x10）
    reinterpret_cast<SendResultView*>(&result)->m_04 = static_cast<char>(
        user->Return_Mercenary_Charac(reinterpret_cast<MercView*>(&req)->m_field10,
                                      result.m_result));
    reinterpret_cast<SendResultView*>(&result)->m_05 =
        reinterpret_cast<MercView*>(&req)->m_field0d;      // +0x05 char 回显(msg+0xd)
    reinterpret_cast<SendResultView*>(&result)->m_08 =
        reinterpret_cast<MercView*>(&req)->m_field10;      // +0x08 int = msg+0x10
    return 0;
}