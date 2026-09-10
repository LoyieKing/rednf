unsigned int Dispatcher_CloseDisjointStore::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)msg;
    (void)param;
    // b28c-33：ORIG 为短路条件的 bool 物化岛（jle→mov$1 / jne→mov$0 / test al;je）。
    // 4.4 对 `!(a&&b)` 折成 (a&&b) 方向物化（jle→mov$0 / je→mov$0，块序相反）；
    // 改等价德摩根形 `a<=2 || b==0`（探针 l33e f_or）逐条复现 ORIG。
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return -1;
    }
    if (user->GetCurCharacExpertJobType() != 3)
    {
        return -2;
    }
    if (user->GetCurCharacExpertJob() == 0)
    {
        return 0x13;
    }
    if (user->GetCurCharacExpertJob()->GetType() != 3)
    {
        return 0x13;
    }
    return 0;
