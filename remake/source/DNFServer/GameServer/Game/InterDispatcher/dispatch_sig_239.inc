// ORIG 0x084e26b0 T dispatch_sig —— 线上预选队伍成员进入。
// 报告 docs/class_func_reports/Inter_LoadOnlinePreliminaryTeam/dispatch_sig.md 全文转写。
int Inter_LoadOnlinePreliminaryTeam::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)
        return 0;
    if (user->getCurCharacR() == 0)
        return 0;
    if (!GlobalData::s_onlinePreliminaryTeamMgr->OnEnterUser(user, data))
        return 0x5219;
    return 0;
}