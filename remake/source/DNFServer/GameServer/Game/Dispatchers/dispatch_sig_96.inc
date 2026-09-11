// b28d-solo-Dispatchers verdict=DIFF(routed) [N96] ORIG 0x0820697c orig/ours=118/118 ndiff-blocks=15
//   P0 清绑：原 `register asm("reg")` 存量绑定已删（N96 全域禁绑，不得恢复）；
//   清绑后残余寄存器/槽序差按 N96+N18a 封顶（实测内联/unsigned 化等自然形不闭合）。
//   首差: sub    $0x38,%esp | movl   $&_ZZN29Dispatcher_ChangeTutorialFlag12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | movl   $0x0,-0x14(
int Dispatcher_ChangeTutorialFlag::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() < 2)
    {
        return LineFunc(0xa115,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    unsigned int flag = 0;
    if (!packet.get_int(flag))
    {
        return LineFunc(0xa118,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    char rewardFlag = 0;
    if (!packet.get_byte(rewardFlag))
    {
        return LineFunc(0xa11b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (flag > 0x4d)
    {
        return LineFunc(0xa120,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // ORIG 0x8206a8b：`test %eax,%eax; setne %al; test %al,%al; je <尾岛>`（!=0 物化
    // 真值，false→return 0）—— 物化值**无栈槽**（P0）。具名 bool 会多占 -0x11 并把
    // 帧从 0x38 顶到 0x48（4.4 带 call 帧恒 16k+4 量化）⇒ 改内联条件形。
    if (user->getCurCharacR() != 0)
    {
        if (flag == 0x4d)
        {
            // ORIG 0x8206abf：cmpl $0x4d; setle（带符号；int→unsigned 实参先装，技巧5）。
            for (int i = 0; i <= 0x4d; i = i + 1)
            {
                user->setCurCharacTutorialFlag(i);
            }
            return 0;
        }
        if (user->setCurCharacTutorialFlag(flag) != 1)
        {
            return 0;
        }
        if (rewardFlag != 0)
        {
            user->RewardTutorial(flag);
        }
        if (flag == 0x1f)
        {
            CParty* party = (CParty*)user->GetParty();
            if (party != 0)
            {
                party->giveup_game(user, true, false, false);
            }
        }
    }
    return 0;
}