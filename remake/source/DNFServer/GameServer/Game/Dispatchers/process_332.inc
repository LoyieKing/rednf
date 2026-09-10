int Dispatcher_SoloTeleport::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_SOLO_TELEPORT& req = reinterpret_cast<MSG_SOLO_TELEPORT&>(msg);
    stSoloTeleportParam& res = static_cast<stSoloTeleportParam&>(param);

    // ORIG:m_44 存全局槽地址(&g_pTeleport[0] 或 &g_pTeleport[1],即 Teleport**;
    // GlobalData.h:namespace GlobalData { extern void* g_pTeleport[2]; } @0x0941f818)
    req.m_44 = (int)(req.m_34 != -1 ? &GlobalData::g_pTeleport[0]
                                    : &GlobalData::g_pTeleport[1]);

    // ORIG 0x081e1dc7: STTeleportData data; data.m_field00=req.m_30;
    //   data.m_tp 前 10 字节 = req.m_tp(state..posy 按 4B/4B/2B 三段展开拷贝);
    //   (*Teleport**)(req.m_44)->set_teleport_data(data)（vtable 槽3 间接调用）
    STTeleportData data;
    data.m_field00 = req.m_30;
    data.m_tp = req.m_tp;
    (*reinterpret_cast<Teleport**>(req.m_44))->set_teleport_data(data);

    const int err = check_error(user, msg, param);
    if (err > 0)
    {
        res.m_4 = err;
    }
    else if (err < 0)
    {
        return -1;   // ORIG:err<0 → 返回 -1(dispatch_template result<0 → 不发包)
    }

    if (err == 0)
    {
        // ORIG 0x081e1e7b: (*Teleport**)(req.m_44)->set_end_cool_time(user)
        (*reinterpret_cast<Teleport**>(req.m_44))->set_end_cool_time(user);
        res.m_8 = req.m_30;   // ORIG:param+8 = msg+0x30(err==0 时)
    }

    res.m_c = req.m_44;       // ORIG:param+0xc = msg+0x44(无条件)
    return 0;
