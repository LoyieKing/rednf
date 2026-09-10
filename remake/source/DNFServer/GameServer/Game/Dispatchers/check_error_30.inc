int Dispatcher_ItemDictionary::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081dd880：user!=0 正向嵌套 + setle 物化（技巧9）+ msg 死槽 -0xc（技巧13 声明位）。
    // 技巧23 双 goto：4.4 需把 user==0 的 -1 岛摆在入口 cond 之后（ORIG jne BODY 形）。
    if (user != 0)
        goto LBODY;
    return -1;
LBODY:
    if ((bool)(user->get_state() <= 2))
        return 1;
    MSG_BASE* pm = &msg;
    (void)pm;
    return 0;
