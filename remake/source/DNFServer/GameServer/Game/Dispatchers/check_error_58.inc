unsigned int Dispatcher_RepairDisjointMachine::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)msg;
    (void)param;
    // ORIG 0x081cfddc（报告 Dispatcher_RepairDisjointMachine/check_error.md 全文已读）：
    // || 短路 bool 物化（jne→mov $0 merge）→ -1；ExpertJobType!=3 经 (bool) setne → -2。
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return -1;
    }
    if ((bool)(user->GetCurCharacExpertJobType() != 3))
    {
        return -2;
    }
    return 0;
