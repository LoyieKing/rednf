// b28d-solo-Dispatchers verdict=AE(routed) [S162-verify] ORIG 0x081e4ec8 orig/ours=21/21
int Dispatcher_LogIngameAdvertisement::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081e4ece：user!=0 正向嵌套 + setle 物化（技巧9）+ msg 死槽 -0xc。
    // 技巧23：入口岛须摆在 cond 之后（ORIG jne BODY 块序）。
    if (user != 0)
        goto LBODY;
    return -1;
LBODY:
    if ((bool)(user->get_state() <= 1))
        return 0x7fffffff;
    MSG_BASE* pm = &msg;
    (void)pm;
    return 0;
}