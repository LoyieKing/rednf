int Dispatcher_MotionHack::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;
    MSG_MOTION_HACK& req = reinterpret_cast<MSG_MOTION_HACK&>(msg);
    if (req.m_field0d != 0)
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x83, 1u,
            (unsigned int)req.m_field0e, (unsigned int)req.m_field12);
    }
    return 0;
