int DisPatcher_Login::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x081e8c78 静态局部 garenaToken（801B，BSS 无初始化；每次调用 memset 清零）
    static char garenaToken[0x801];

    ST_LOGIN_AUTH_PAYLOAD authBuf;       // -0x1a7..-0xe9（ORIG memset 0xbf 清零）
    unsigned int patchTime;              // -0x84
    unsigned int tokenLen;               // -0x88
    int versionMajor;                    // -0x8c
    char decPwd[8];                      // -0x94
    unsigned int version;                // -0x98
    int nowSec;                          // -0x10
    unsigned int rentField;              // -0xa0

    if (user->get_state() != 1)
    {
        PacketGuard packetGuard;
        packetGuard.put_header(1, 3);
        packetGuard.put_byte(1);
        packetGuard.finalize(true);
        user->Send(packetGuard);
        return 0;
    }

    user->OnRecvEvent();
    if (G_GameWorld()->GetChannelType() == 6)
    {
        CEventBase* repeatEvent = GlobalData::s_event_manager->GetRepeatEvent(0x21);
        if (!repeatEvent->IsEventing((CUser*)0))
        {
            PacketGuard packetGuard;
            packetGuard.put_header(1, 1);
            packetGuard.put_byte(0);
            packetGuard.put_byte(0x42);
            packetGuard.finalize(true);
            user->Send(packetGuard);
            cMyTrace trace(__PRETTY_FUNCTION__, 0x65f4, 0);
            trace("Can't Enter Into Guild War Channel ");
            user->DisConnSig((DISCONN_SIG)0x16, true, 0);
            return 0;
        }
    }

    memset(&authBuf, 0, sizeof(authBuf));
    patchTime = 0;
    if (packet.get_int(patchTime) != 1)
        return LineFunc(0x6601, __PRETTY_FUNCTION__, 0, 0);

    if (0xe10 < patchTime)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x6605, 5);
        trace("[Taiwan, Auth] Invalid patch time. (time:%d,limit:%d)",
              patchTime, 0xe10);
        return LineFunc(0x6607, __PRETTY_FUNCTION__, 0, 0);
    }

    tokenLen = 0;
    if (packet.get_int(tokenLen) != 1)
        return LineFunc(0x660d, __PRETTY_FUNCTION__, 0, 0);

    memset(garenaToken, 0, 0x801);
    if (packet.get_str(garenaToken, 0x801, (int)tokenLen) != 1)
        return LineFunc(0x6612, __PRETTY_FUNCTION__, 0, 0);

    // P48/P0：ORIG 每用点独立求值（Ghidra 4 次 getGarenaAuthData：decrypt/setPatchTime/
    // getPatchTime/getTimeStamp 各一），禁缓存 authData。
