// ==== Dispatcher_SecurityCardAuthRpy::check_error (ORIG 0x0826314c) ====
int Dispatcher_SecurityCardAuthRpy::check_error(CUser* user, MSG_BASE& msg)
{
    ScrMsgView& m = *reinterpret_cast<ScrMsgView*>(&msg);
    if (m.m_cryptoType == 0 || m.m_cryptoType > 0xd || m.m_cryptoType == 5)
    {
        // ORIG 0x082631?: 本分支 trace 落 -0x2c（远槽）、state 分支 -0x1c。
        // 技巧3 注册逆序：临时对象直调形改回退分配序。
        cMyTrace(__PRETTY_FUNCTION__, 0x14c, 0)(
            "ERROR [TRACE AUTH RPY] : crypto_type2");
        return 0x14d;
    }
    if (user->get_state() <= 1)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x154, 0)(
            "ERROR [TRACE AUTH RPY] : get_state()<LOGIN");
        return 0x155;
    }
    return 0;
}