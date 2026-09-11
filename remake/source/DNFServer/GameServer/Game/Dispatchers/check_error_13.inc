// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0825f3ce orig/ours=41/40 ndiff-blocks=3
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: je     0825f43c <+0x6e> | jmp    0825f441 <+0x73> | mov    $0x0,%eax
int Dispatcher_ComboSkill::check_error(CUser* user, MSG_BASE& msg)
{
    const MSG_COMBO_SKILL& req = reinterpret_cast<const MSG_COMBO_SKILL&>(msg);
    // b28d-D0: P133 拆独立守卫 —— ORIG 0x0825f3e7 段三条件各自物化
    //   （user==0 直跳；state<3 出 setle；job!=9 出 setne）；合并 || 链被 4.4 折成直跳。
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() < 3)
    {
        return -1;
    }
    // b28d-D0: P226 语句表达式（无具名变量）复现 ORIG 0x0825f3f3 `setne %al;
    //   test %al,%al; je` 免槽物化（具名 bool 会 spill 到 -0x9）。
    if (({ user->get_charac_job() != 9; }))
    {
        return -1;
    }
    // b28d-solo-Dispatchers: ORIG 0x0825f41f 两条件各自 `je <mov $0,%eax>` 共享岛
    //   （负极性 && 链），正极性 || 被 4.4 折成 `jne <mov $0x13>` 反布局（P214①/P45）。
    if (req.m_pad0d != 0 && req.m_pad0d != 1)
    {
        return 0x13;
    }
    return 0;
}