// ORIG 0x081cff34（nm 实测，sz 0x5f）：state>2 且 curCharac!=0；ExpertJobType!=3 → -2。
unsigned int Dispatcher_UpgradeDisjointMachine::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)msg;
    (void)param;
    // ORIG 0x081cff34（报告 Dispatcher_UpgradeDisjointMachine/check_error.md 全文已读）：
    // 同 Repair 形态：|| 短路 bool 物化 → -1；(bool) setne 物化 → -2；否则 0。
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return -1;
    }
    if ((bool)(user->GetCurCharacExpertJobType() != 3))
    {
        return -2;
    }
    return 0;
}