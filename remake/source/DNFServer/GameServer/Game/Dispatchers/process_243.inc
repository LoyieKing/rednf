int Dispatcher_LogIngameAdvertisement::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081e4df2：check_error 结果写 param+0x04；
    // err>0 → 0；err<0 → LineFunc(0x5d83)；err==0 → 广告管理器写日志。
    int err = check_error(user, msg);
    *(int*)(reinterpret_cast<char*>(&param) + 0x4) = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        LineFunc(0x5d83,
                 __PRETTY_FUNCTION__,
                 err, 0);
        return 0;
    }

    // ORIG 字段读取顺序：+0x11 uint → +0x10 char(movsbl) → +0xe short(movswl)
    // → +0xd char(movsbl)。
    MSG_LOG_INGAME_ADVERTISEMENT& mref =
        reinterpret_cast<MSG_LOG_INGAME_ADVERTISEMENT&>(msg);
    unsigned int extra = mref.m_field11;                       // +0x11 uint
    signed char flag =
        (signed char)(*reinterpret_cast<unsigned char*>(&mref.m_field10));  // +0x10
    short val = mref.m_field0e;                                // +0x0e short
    signed char type =
        (signed char)(*reinterpret_cast<unsigned char*>(&mref.m_field0d));  // +0x0d

    CInGameAdvertisementManager* mgr = InGameADManagerInstance();
    mgr->WriteLog(user, (char)type, val, (char)flag, extra);
    return 0;
