void AvatarRechargeServer::GmChangeEnduranceReduceRate(CUser* user, bool enable,
                                                       int rate)
{
    if (!user->isGMUser())
        return;
    // ORIG 0x08191318：静态（无 this），经 Singleton::Get 取实例后转发
    ARAD::Singleton<AvatarRechargeServer>::Get()->GmSetEnduranceReduceRate(
        user, enable, rate);
