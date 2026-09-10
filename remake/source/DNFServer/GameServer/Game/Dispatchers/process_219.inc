int Dispatcher_PrecheckSoloTeleport::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081e19ca：check_error(三参)；
    // err>0 → 写 param+0x04（继续）；err<0 → 返回 -1；err==0 → 发 monitor 预检包。
    // ORIG 0x081e19d1: `mov 0x14(%ebp),%eax; mov %eax,-0x14(%ebp)` —— param 引用槽；
    //   0x081e19d7: `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)` —— msg 引用槽；
    //   0x081e1a2f 长度读 `mov 0x2c(%eax),%eax` 走该 msg 引用常量偏移。
    stSoloTeleportParam& out = static_cast<stSoloTeleportParam&>(param);
    MSG_SOLO_TELEPORT& m = reinterpret_cast<MSG_SOLO_TELEPORT&>(msg);
    int err = check_error(user, msg, param);
    if (err > 0)
    {
        // ORIG 0x081e1a06: `mov -0x14(%ebp),%eax; mov -0xc(%ebp),%edx; mov %edx,0x4(%eax)`
        //（原 `*(int*)((char*)&param+0x4)=err` 产 `lea 0x4(%eax),%edx` 形）
        out.m_4 = err;
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
                reinterpret_cast<char*>(&m) + 0xd,
                *(int*)(reinterpret_cast<char*>(&m) + 0x2c));
        *(int*)(reinterpret_cast<char*>(&pkt) + 0x1f) = user->getCurCharacNo();

        ENUM_SERVER_GROUP group = user->GetServerGroup();
        CMonitorServerProxy* proxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(group);
        proxy->SendPacket(reinterpret_cast<char*>(&pkt), 0x23);
    }
    return 0;
