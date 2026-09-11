// ===== InterLoadHeroMissionEventData::dispatch_sig —— ORIG 0x08161c92 =====
// buf==0 → cMyTrace(0x1ff) 日志 "[HeroMission] msg is null." 返回 0x200；
// setEventData(user, buf+4)；IsFirstTimeLogin 命中 → processMission(user, 5, 0)；
// sendMissionData(user)；返回 0。
// [b28d-solo-TaiwanInternalDispatcher] AE（sig 提前 + Get() 链式 + IsFirstTimeLogin bool）
int InterLoadHeroMissionEventData::dispatch_sig(CUser* user, char* buf, int len)
{
    SigLoadHeroMissionEventData* sig =
        reinterpret_cast<SigLoadHeroMissionEventData*>(buf);
    if (sig == 0)
    {
        cMyTrace tr(__PRETTY_FUNCTION__, 0x1ff, 5);
        tr("[HeroMission] msg is null.");
        return 0x200;
    }
    ARAD::Singleton<HeroMissionEvent>::Get()->setEventData(user, sig->data);
    if (user->IsFirstTimeLogin())
    {
        ARAD::Singleton<HeroMissionEvent>::Get()->processMission(
            user, HeroMissionCondition::MissionNo::T_5, 0);
    }
    ARAD::Singleton<HeroMissionEvent>::Get()->sendMissionData(user);
    return 0;
}