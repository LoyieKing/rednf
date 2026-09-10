unsigned int Dispatcher_RequestMemberEnter::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081cb830：无 user 判空；bool 双岛物化（技巧22/23 goto 版：
    // 普通 if/else 在 4.4 折成 setle/sete+test，双 goto 保 jle/jne 直分支）：
    // state<=2 || characR==0 → fail → -1；成功路径 0x081cb866 才物化 msg
    // dead 槽 -0xc（技巧13 声明位=落槽时机）再返 0。
    // [b28d-solo-S162] P226：取反短路条件表达式直用（禁 register 绑定，P0）。
    if (!(user->get_state() > 2 && user->getCurCharacR() != 0))
    {
        return -1;
    }
    MSG_BASE* const pm = &msg;
    (void)pm;
    return 0;
