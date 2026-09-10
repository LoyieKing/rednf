int Inter_CutOffCallUserInfo::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    if (G_CGameManager() == 0)
    {
        // ORIG 0x084c8a84-084c8aa2：null 支 trace 槽=-0x2c（深）、send 支=-0x1c（浅）
        // ⇒ 临时对象注册逆序分配（技巧3；probe cutoff w3：具名局部则正序、反 ORIG）。
        cMyTrace(__PRETTY_FUNCTION__, 0x2249, 5)(
            "Inter_MonitorCallUserInfo::dispatch_sig()  G_CGameManager() is null");
    }
    else
    {
        char* p = data;
        G_CGameManager()->send_userinfos_to_cutoff_server();
        cMyTrace(__PRETTY_FUNCTION__, 0x2252, 0)(
            "Cut Off Server Call UserInfo Arrived\n");
    }
    return 0;
