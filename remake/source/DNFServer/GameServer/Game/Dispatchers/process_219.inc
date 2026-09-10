int Dispatcher_PrecheckSoloTeleport::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081e19ca：check_error(三参)；
    // err>0 → 写 param+0x04（继续）；err<0 → 返回 -1；err==0 → 发 monitor 预检包。
    int err = check_error(user, msg, param);
    if (err > 0)
    {
        *(int*)(reinterpret_cast<char*>(&param) + 0x4) = err;
    }
    else if (err < 0)
    {
        return 0xffffffff;
    }

    if (err == 0)
    {
        // ORIG 0x0822c5d2 ctor；+0x0a 起 21 字节名字区 strncpy(msg+0xd, 长度
        // *(int*)(msg+0x2c))；+0x1f int = getCurCharacNo(0x080cbc4e)；SendPacket 0x23。
        Packet_Monitor_Request_Charac_Info pkt;
        strncpy(reinterpret_cast<char*>(&pkt) + 0xa,
                reinterpret_cast<char*>(&msg) + 0xd,
                *(int*)(reinterpret_cast<char*>(&msg) + 0x2c));
        *(int*)(reinterpret_cast<char*>(&pkt) + 0x1f) = user->getCurCharacNo();

        ENUM_SERVER_GROUP group = user->GetServerGroup();
        CMonitorServerProxy* proxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(group);
        proxy->SendPacket(reinterpret_cast<char*>(&pkt), 0x23);
    }
    return 0;
