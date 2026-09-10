int Dispatcher_Teleport::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stTeleportParam& out = static_cast<stTeleportParam&>(param);
    MSG_TELEPORT& req = reinterpret_cast<MSG_TELEPORT&>(msg);

    out.m_4 = check_error(user, msg, param);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x2bcc,
                        __PRETTY_FUNCTION__,
                        out.m_4, user->get_acc_id());
    }
    *reinterpret_cast<short*>(&out.m_gapA[0]) = req.m_field0e;
    *reinterpret_cast<unsigned int*>(&out.m_gapA[2]) =
        req.m_field10;
    out.m_10 = req.m_field14;
    out.m_11 = req.m_field15;
    return 0;
