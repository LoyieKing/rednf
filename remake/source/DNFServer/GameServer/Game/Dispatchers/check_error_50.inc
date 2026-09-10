unsigned int Dispatcher_SaveCharacterOption::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() < 2)
    {
        return -2;
    }
    if ((bool)(user->getCurCharacNo() == 0))   // 同上：sete 物化保形
        return -3;
    return 0;
