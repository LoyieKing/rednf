int Dispatcher_RequestIngameAdvertisement::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;                                          // 0xffffffff
    }

    if (user->get_state() <= 1)                             // cmp $1,setle（P2 有符号比较）
    {
        return 0x7fffffff;
    }

    // ORIG 0x081e4691：msg 指针先物化 -0xc 槽再单级 disp 读（具名指针局部，
    // 技巧13 落栈时机=声明位置）；`cmp $0x6,%ax` 16 位寄存器比较；failCnt
    // <=2 走 bool 物化 `cmp $2; setle %al; test`（技巧9/19，勿写 <3 被折 jg）。
    MSG_REQUEST_INGAME_ADVERTISEMENT* p =
        reinterpret_cast<MSG_REQUEST_INGAME_ADVERTISEMENT*>(&msg);
    if (p->m_field0d == 6)
    {
        // ORIG 0x081e46aa：`call; cmp $2,%eax; setle %al; test %al,%al; je`
        //   （寄存器物化、零槽）。直写 `(bool)` 被 4.4 折 jg+mov$1/$0 岛；
        //   具名 bool 落槽（-0x9 存取对）并使 p 槽 -0xc→-0x10。
        // [b28d-solo-S206] P316/P226 语句表达式形（内不含具名变量）零槽复现。
        if (({ user->get_multiboxLotteryItemFailCnt() <= 2; }))
        {
            return -2;                                  // 0xfffffffe
        }
    }

    return 0;
