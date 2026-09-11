// ============================================================================
// 4. Dispatcher_SelectUltimateDifficulty::process(CUser*, MSG_BASE&, ParamBase&)
//    ORIG @0x0823426c W：push ebp; mov esp,ebp; mov $0x0,%eax; pop ebp; ret —— 空体。
//    实际业务逻辑在 dispatch_sig（Dispatchers.cpp 已还原）。
// ============================================================================
int Dispatcher_SelectUltimateDifficulty::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)user; (void)msg; (void)param;
    return 0;
}