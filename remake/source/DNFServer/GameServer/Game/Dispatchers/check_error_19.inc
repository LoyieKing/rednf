// ORIG 0x081cc2a2（报告 Dispatcher_MailBox_Open/check_error.md）
unsigned int Dispatcher_MailBox_Open::check_error(CUser* user, MSG_BASE& msg)
{
    (void)msg;
    if (user->get_state() != 3 || user->getCurCharacR() == 0)
    {
        return -1;   // ORIG 0x081cc2e6：返回 -1（走 LineFunc 错误路径）
    }
    return 0;
}