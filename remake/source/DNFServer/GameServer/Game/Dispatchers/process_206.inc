int Dispatcher_AbnormalUseStackable::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;
    MSG_ABNORMAL_USE_STACKABLE& req = reinterpret_cast<MSG_ABNORMAL_USE_STACKABLE&>(msg);
    int err = check_error(user, msg);
    if (err == 0)
    {
        WongWork::CHackAnalyzer* analyzer = user->getHackAnalyzer();
        analyzer->addServerHackCnt(user, (WongWork::ENUM_HACK_TYPE)0xfa0, 1,
                                   (unsigned int)req.m_field10, (unsigned int)req.m_field14);
    }
    return 0;
