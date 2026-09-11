// ============================================================================
// GmChangeEnduranceReduceRate —— ORIG T 0x08191318
// GM 前置校验后转发 GmSetEnduranceReduceRate（ORIG 经 Singleton::Get 取 this，
// Meyers 单例同一对象，语义一致）。
// ============================================================================
void AvatarRechargeServer::GmChangeEnduranceReduceRate(CUser* user, bool enable,
                                                       int rate)
{
    if (!user->isGMUser())
        return;
    // ORIG 0x08191318：静态（无 this），经 Singleton::Get 取实例后转发
    ARAD::Singleton<AvatarRechargeServer>::Get()->GmSetEnduranceReduceRate(
        user, enable, rate);
}