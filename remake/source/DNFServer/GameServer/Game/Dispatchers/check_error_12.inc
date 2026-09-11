// b28d-solo-Dispatchers verdict=AE(routed) [P226/P0] ORIG 0x081e612c orig/ours=62/62
//   [S162] 已修：尾部具名 bool（fail = !(get_state()==0xb || ==5)）落 -0x9 并使
//   全帧槽号 +4；改条件表达式直用（ORIG 物化于 %al `test %al,%al`，P0 无槽往返）。
int Dispatcher_BlueMarble::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081e60xx 入口即 `mov 0x10(%ebp),%eax; mov %eax,-0x14(%ebp)` —— msg 引用槽
    // 在 user 判空**之前**物化（技巧13 声明位=落栈时机）；我方原放在判空之后 ⇒ 前 5 行错位。
    const MSG_BLUE_MARBLE& req = reinterpret_cast<const MSG_BLUE_MARBLE&>(msg);
    if (user == 0)
    {
        return -1;
    }
    if (req.m_0d == 6)
    {
        return 0;
    }
    if (user->getMoveSpace() != (UserSpace::T)1)
    {
        return -2;
    }
    BlueMarble* marble = static_cast<BlueMarble*>(user->getBlueMarble());
    if (marble == 0)
    {
        return -3;
    }
    // ORIG：`mov %eax,-0xc(%ebp); cmpl $-1,-0xc(%ebp); jne` —— 结果先落具名槽 -0xc、
    // 再用 mem 作 src 直分支。我方把 call 结果内联比较（技巧37 免槽形）在此点位反被
    // 4.4 折成 `cmp $-1,%eax; sete;test;je` 岛 ⇒ 此处必须建具名局部。
    const int slot = marble->getUserSlot(user);
    if (slot == -1)
    {
        return -4;
    }
    // 【batch-17 S池 P48+P84】ORIG 两臂各调一次 get_state，结果 mov$1/mov$0 物化
    // 成 bool 再 test/je 选 return（0/0xfffffffb）。具名 bool 保物化岛。
    // [b28d-solo-S162] P226/P0：ORIG 物化于 %al（`test %al,%al`）无槽往返
    // ⇒ 条件表达式直用；具名 bool 落 -0x9 并使全帧槽号 +4 漂移。
    if (!(user->get_state() == 0xb || user->get_state() == 5))
    {
        return -5;
    }
    return 0;
}