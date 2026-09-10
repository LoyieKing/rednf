int Dispatcher_ValidateScriptHash::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;
    if (check_error(user, msg) != 0)
    {
        return 0;
    }
    MSG_SCRIPT_HASH& req = reinterpret_cast<MSG_SCRIPT_HASH&>(msg);
    const int hackType = get_hack_type((char)req.m_field0d);
    user->getHackAnalyzer()->addServerHackCnt(
        user, (WongWork::ENUM_HACKTYPE)hackType, 1u,
        (unsigned int)req.m_field0e, (unsigned int)req.m_field0f);
    return 0;
