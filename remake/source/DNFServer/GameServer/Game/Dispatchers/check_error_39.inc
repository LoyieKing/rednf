unsigned int Dispatcher_MemberEnterReply::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x81cba01：无 user==0；`state<=2 || charac==0` 短路→bool 合并岛 mov$1/$0。
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return -1;
    }
    return 0;
}