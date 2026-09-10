int Dispatcher_PvpMissionHPPercent::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    if (user->getCurCharacR() == 0)
    {
        return LineFunc(0x3930,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    MSG_PVP_MISSION_HP_PERCENT& req = reinterpret_cast<MSG_PVP_MISSION_HP_PERCENT&>(msg);
    stSendClearedMission& cleared = static_cast<stSendClearedMission&>(param);
    user->UpdateMissionCondition(req.m_hpPercents, cleared.m_missionIdxs);
    return 0;
