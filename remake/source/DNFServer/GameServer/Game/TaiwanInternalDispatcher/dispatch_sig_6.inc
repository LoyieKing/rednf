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
