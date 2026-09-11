// ============================================================================
// 12. Dispatcher_PrecheckSoloTeleport::check_error
//     报告 docs/class_func_reports/Dispatcher_PrecheckSoloTeleport/check_error.md
// ============================================================================
int Dispatcher_PrecheckSoloTeleport::check_error(CUser* user, MSG_BASE& msg,
                                                 ParamBase& param)
{
    if (user == 0)
        return -1;
    int result = 0;                       // ORIG：先 0 初始化再赋 check_rule 结果（栈槽 -0xc）
    result = Teleport::check_rule(user);
    return result;
}