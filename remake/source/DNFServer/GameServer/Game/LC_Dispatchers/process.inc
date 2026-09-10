int Dispatcher_MouseRegister::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    TempParam& out = static_cast<TempParam&>(param);
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x178,
                        __PRETTY_FUNCTION__,
                        out.m_4, 0);
    }
    MSG_BASE* dead = &msg;
    (void)dead;
    return 0;
