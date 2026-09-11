// ORIG 0x081cbbd0（报告 Dispatcher_MemberSecede/check_error.md）：
// state<=2 或 curCharac==0 → -1；get_charac_memberkey()==0 → 0x7fffffff；否则 0。
unsigned int Dispatcher_MemberSecede::check_error(CUser* user, MSG_BASE& msg)
{
    (void)msg;
    // ORIG 0x081cbbd0（报告 Dispatcher_MemberSecede/check_error.md 全文已读）：
    // || 短路 bool 物化（mov $1/$0 merge + test %al）→ -1；
    // memberkey==0 经 (bool) sete 物化 → 0x7fffffff；否则 0。
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return -1;
    }
    if ((bool)(user->get_charac_memberkey() == 0))
    {
        return 0x7fffffff;
    }
    return 0;
}