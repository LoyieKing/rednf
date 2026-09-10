unsigned int Dispatcher_ChangeLetterStat::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081ce2c0：首行 msg 指针 dead 槽 -0xc（技巧8）；bool 双岛物化
    // `mov $1/jmp/mov $0` 于 %al（技巧22/23：双 goto 保 jle/jne 直分支，
    // 普通 if/else 会被 4.4 折成 setle/test+sete/test）：
    // state<=2 || characR==0 → fail；fail → -1，否则 0。
    MSG_CHANGE_LETTER_STAT* const pm =
        reinterpret_cast<MSG_CHANGE_LETTER_STAT*>(&msg);
    (void)pm;
    // [b28d-solo-S162] P226：取反短路条件表达式直用（禁 register 绑定，P0）。
    if (!(user->get_state() > 2 && user->getCurCharacR() != 0))
    {
        return -1;
    }
    return 0;
