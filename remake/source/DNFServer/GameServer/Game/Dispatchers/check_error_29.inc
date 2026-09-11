unsigned int Dispatcher_CallMemberList::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x81cbd44：当前角色及 memberkey 必须有效（ORIG 无 user 判空）。
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return -1;
    }
    // 探针实证 f1 形：`if ((bool)(call()==0))` 在 4.4 保留 test/sete/test/je 物化
    // （三目与命名局部均被折成分支）。
    if ((bool)(user->get_charac_memberkey() == 0))
        return 0x7fffffff;
    return 0;
}