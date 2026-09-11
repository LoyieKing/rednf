// ORIG 0x081cb274 T _ZN20DisPatcher_AddFriend11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_AddFriend/check_error.md 逐块映射）：state<=2 → 0x7fffffff；
// (msg+0x11) 指针空或与自身角色名 strcmp 相同 → 1；否则 0。
// 注：ORIG 汇编对 (msg+0x11) 地址值做 test/jne（非解引用检查）。
unsigned int DisPatcher_AddFriend::check_error(CUser* user, MSG_BASE& msg)

{
    // ORIG 0x081... 入口 `mov 0x10(%ebp),%eax; mov %eax,-0xc(%ebp)`：view 指针局部在入口声明，
    // 后续 `mov -0xc(%ebp),%eax/%edx; add $0x11` 均从该槽重定位。
    STFriendReqView* req = reinterpret_cast<STFriendReqView*>(&msg);
    if (user->get_state() <= 2)
    {
        return 0x7fffffff;
    }
    if (req->m_name == 0)                                    // ORIG `add $0x11; test %eax,%eax; jne`（死检查保留）
    {
        return 1;
    }
    if (strcmp(req->m_name, user->getCurCharacName()) == 0)
    {
        return 1;
    }
    return 0;
}
