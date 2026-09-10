int Dispatcher_HeroMissionEventState_Reward::dispatch_sig(CUser* user, PacketBuf& packet)
{
    int missionState = 0;
    if (!packet.get_short(*(short*)&missionState))
    {
        return LineFunc(0xb69,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int missionNo = 0;
    if (!packet.get_short(*(short*)&missionNo))
    {
        return LineFunc(0xb6d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (missionState == 1 && missionNo < 0 && missionNo > 6)
    {
        return LineFunc(0xb71,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (missionState == 0)
    {
        HeroMissionEvent* event = ARAD::Singleton<HeroMissionEvent>::Get();
        event->sendMissionData(user);
    }
    else
    {
        HeroMissionEvent* event = ARAD::Singleton<HeroMissionEvent>::Get();
        char rewarded = (char)event->rewardMission(
            user, (HeroMissionCondition::MissionNo::T)missionNo);
        PacketGuard response;
        response.put_header(1, 0x25a);
        if (rewarded == 0)
        {
            response.put_byte(0);
            response.put_byte(0);
            response.put_short(missionState);
        }
        else
        {
            response.put_byte(1);
            response.put_short(missionState);
        }
        response.finalize(true);
        user->Send(response);
    }
    return 0;
