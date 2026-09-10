int DisPatcher_Login::dispatch_sig(CUser* user, PacketBuf& packet)

{
    // [H5DisPatcher 核账 2026-09-11] 实测 ours 989→981 ✅（repeatEvent / patchTimeGot+tokenStamp / userIdStr 三处具名量内联）
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
        // [DPC_SigA] P0/P183：ORIG 0x081e8d75 `call GetRepeatEvent` 之后
        // 直接 `mov (%eax),%edx; add $0x34,%edx; mov (%edx),%edx; ...;
        // mov %eax,(%esp); call *%edx`（结果**不经槽**，%eax 贯穿两 call）。
        // 原具名 CEventBase* repeatEvent 多出「存槽 + 取槽」2 条并占 4B 帧。
        if (!GlobalData::s_event_manager->GetRepeatEvent(0x21)->IsEventing((CUser*)0))
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
#define GARENA_AUTH reinterpret_cast<Taiwan::GarenaAuthData*>(user->getGarenaAuthData())
    if (Taiwan::AuthGarenaToken::decrypt(garenaToken, (int)tokenLen,
                                         *GARENA_AUTH) != 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x6617, 5);
        trace("[Taiwan, Auth] Fail decrypt:%s", garenaToken);
        return LineFunc(0x6618, __PRETTY_FUNCTION__, 0, 0);
    }

    GARENA_AUTH->setPatchTime(patchTime);
    nowSec = GlobalData::s_systemTime_.getCurSec();
    // [DPC_SigA] P0/P183：ORIG 0x081e912f..0x081e9179 两个取样值**均无具名槽** ——
    // `call getPatchTime` → `%esi`、`call getTimeStamp` → `%ebx`（callee-saved 跨
    // ctor call 保活），装参 `%esi→0x10(%esp)`/`%ebx→0x8(%esp)`；实参右→左求值
    // （先 patchTime 后 timestamp，与 gcc 4.4.7 -O0 实证一致）。原两个具名
    // unsigned int 多出 store/load 槽往返各 2 条并占 8B 帧。
    cMyTrace trace(__PRETTY_FUNCTION__, 0x661e, 0);
    trace("[Taiwan, Auth] Token timestamp info. (token_time:%u, current_time:%u, patch_time:%d)",
          GARENA_AUTH->getTimeStamp(), nowSec, GARENA_AUTH->getPatchTime());
#undef GARENA_AUTH

    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 0x21))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)1, 0xd1);
        return 0;
    }

    if (packet.get_int(authBuf.m_userIdLen) != 1)
        return LineFunc(0x665f, __PRETTY_FUNCTION__, 0, 0);

    if (packet.get_str(authBuf.m_userId, 0x18,
                       (int)authBuf.m_userIdLen) != 1)
        return LineFunc(0x667d, __PRETTY_FUNCTION__, 0, 0);

    if (packet.get_int(authBuf.m_len2d) != 1)
        return LineFunc(0x6692, __PRETTY_FUNCTION__, 0, 0);

    if (0x83 < authBuf.m_len2d)
        return LineFunc(0x6698, __PRETTY_FUNCTION__, 0, 0);

    if (packet.get_str(authBuf.m_field2d, 0x84,
                       (int)authBuf.m_len2d) != 1)
        return LineFunc(0x66a8, __PRETTY_FUNCTION__, 0, 0);

    // ORIG 0x081e9361/0x081e93ad：登录型别无独立局部，三处判定/末段写 SIG+0xa0
    // 均逐次重读载荷字节（movzbl -0xf6(%ebp)）——P86 逐次重读口径。
    if (packet.get_byte(authBuf.m_loginType) != 1)
        return LineFunc(0x6734, __PRETTY_FUNCTION__, 0, 0);

    if (authBuf.m_loginType != 0 && authBuf.m_loginType != 1 &&
        authBuf.m_loginType != 2)
        return LineFunc(0x6738, __PRETTY_FUNCTION__, 0,
                        (unsigned int)user->get_acc_id());

    versionMajor = 0;
    if (packet.get_int(versionMajor) != 1)
        return LineFunc(0x673f, __PRETTY_FUNCTION__, 0,
                        (unsigned int)user->get_acc_id());

    if (versionMajor != 8)
        return LineFunc(0x6741, __PRETTY_FUNCTION__, 0,
                        (unsigned int)user->get_acc_id());

    if (packet.get_binary(authBuf.m_encPwd, 8) != 1)
        return LineFunc(0x6742, __PRETTY_FUNCTION__, 0, 0);

    if (!Dispatcher_MouseRegister::decryptPassword(
            authBuf.m_encPwd, decPwd, (unsigned int)versionMajor))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)1, 1);
        return LineFunc(0x674e, __PRETTY_FUNCTION__, 0,
                        (unsigned int)user->get_acc_id());
    }

    if (!Dispatcher_MouseRegister::checkValiedPassword(decPwd))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)1, 1);
        return LineFunc(0x6754, __PRETTY_FUNCTION__, 0,
                        (unsigned int)user->get_acc_id());
    }

    memcpy(authBuf.m_encPwd, decPwd, 7);

    version = 0;
    if (packet.get_int(version) != 1)
        return LineFunc(0x675c, __PRETTY_FUNCTION__, 0, 0);

    // CDataManager +0x3c0 = m_serverParameter.m_358（服务端版本字段）
    if (G_CDataManager()->m_serverParameter.m_358 != version)
    {
        PacketGuard versionGuard;                 // -0xdc
        versionGuard.put_header(1, 1);
        versionGuard.put_byte(0);
        versionGuard.put_byte(0x17);
        versionGuard.finalize(true);
        user->Send(versionGuard);
        m_field4 = m_field4 + 1;
        cMyTrace versionTrace(__PRETTY_FUNCTION__, 0x6813, 0);
        versionTrace("version dismatched: count(%d)", m_field4);
        user->DisConnSig((DISCONN_SIG)0x19, false, 0);
        return 0;
    }

    rentField = 0;
    if (packet.get_int(rentField) != 1)
        return LineFunc(0x681b, __PRETTY_FUNCTION__, 0, 0);

    CStreamGuard streamGuard(
        GlobalData::s_big_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp",
                                               0x6972),
        true);                                    // -0xa4
    // ORIG 0x81e9811..0x81e9843（Ghidra de→op<<→de→op<<）：operator* 显式链形。
    streamGuard.operator*()->operator<<(0x3);
    streamGuard.operator*()->operator<<(static_cast<int>(user->GetUID()));
    // 【ABI 镜像】SIG_LOGIN_DATA 偏移字段（SigTypes_D.h 待具名化区：+0x18/+0x84/
    // +0x9c/+0xa0/+0x3905/+0x3a30；+0xa1=m_blackIpCheckBuf、+0x3a34=m_clientVersion、
    // +0x3a38=m_channelRestrictField 已具名可用）
    SIG_LOGIN_DATA* login = streamGuard->GetInBuffer<SIG_LOGIN_DATA>();   // ORIG pt→GetInBuffer 链
    memset(login, 0, 0x4358);
    if (packet.get_short(login->m_channelRestrictField) != 1)
        return LineFunc(0x6980, __PRETTY_FUNCTION__, 0, 0);

    if (login->m_channelRestrictField < 0 || 0x11 < login->m_channelRestrictField)
        login->m_channelRestrictField = 0;

    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        cMyTrace groupTrace(__PRETTY_FUNCTION__, 0x699a, 0);
        groupTrace("CheckLoginServerGroup user_id(%s), server_group(%d)",
                   authBuf.m_userId, (int)login->m_channelRestrictField);
        if ((unsigned short)login->m_channelRestrictField <= 0xa &&
            ((1 << login->m_channelRestrictField) & 0x580) != 0)
            return LineFunc(0x69a2, __PRETTY_FUNCTION__, 0, 0);
    }
    else
    {
        login->m_channelRestrictField = 0;
    }

    user->SetServerGroup((ENUM_SERVER_GROUP)login->m_channelRestrictField);

    int localIp;
    if (packet.get_int(localIp) != 1)
        return LineFunc(0x69cf, __PRETTY_FUNCTION__, 0, 0);
    user->set_local_ip_address(localIp);

    unsigned char logInOutState = 0;
    packet.get_byte(logInOutState);
    user->SetLogInOutState(logInOutState);

    // P48/P0：ORIG 本区两个独立 getGarenaAuthData 求值点（getUid / getTimeStamp 各一）。
    // [DPC_SigA] P0/P183：ORIG 0x081e9a7d..0x081e9ab2 `call getUid` → `mov $0x0,0x4(%esp)`
    // → `call NumberToString` → 结果**不经槽**直接入 strncpy 第 2 实参（Ghidra `__src`
    // 为寄存器临时，local 列表无 char* 槽）⇒ 原具名 char* userIdStr 多余。
    strncpy(login->m_userId,                            // +0x00
            NumberToString(
                reinterpret_cast<Taiwan::GarenaAuthData*>(
                    user->getGarenaAuthData())->getUid(), 0),
            0x18);
    memset(login->m_pad18, 0, 0x84);                    // +0x18 清零区
    login->m_timeStamp =
        reinterpret_cast<Taiwan::GarenaAuthData*>(
            user->getGarenaAuthData())->getTimeStamp(); // +0x3a30

    if (!G_GameWorld()->IsIntegratedPvPBaseChannel())
        login->m_gcNo =
            static_cast<unsigned int>(G_CEnvironment()->m_serverEnvir.m_gcNo);  // +0x9c ← env+0x198
    else
        login->m_gcNo =
            (unsigned int)G_CEnvironment()->get_gc_no_hardcode(
                (ENUM_SERVER_GROUP)login->m_channelRestrictField);

    user->m_network.GetPeerIP2(login->m_blackIpCheckBuf, 0x14);
    // ORIG 0x081e9b64 `movzbl -0xf6(%ebp),%edx; mov %dl,0xa0(%eax)`：载荷字节直读
    login->m_loginType = authBuf.m_loginType;            // +0xa0
    // +0x3905 落于 m_passPadPassword(+0x38e1) 内 +0x24 区（ORIG 网吧密码串写入）
    strncpy(&login->m_passPadPassword[0x24], authBuf.m_encPwd, 7);
    user->SetLoginTime(GlobalData::s_systemTime_.getCurSec());
    // CUser m_mcap 区（+0x8e3f0；字段偏移 = mcap 内偏移）
    user->m_mcap.m_field24 =                           // +0x24（0x8e414）
        (unsigned int)GlobalData::s_systemTime_.getCurSec();
    user->m_network.GetPeerIP2(user->m_mcap.m_pcRoomIp, 0x14);   // +0x28（0x8e418）
    user->m_mcap.m_pad20[0] = 0;                       // +0x20（0x8e410）
    user->m_mcap.m_sellTimeHitCount = 0;               // +0x40（0x8e430）
    user->m_mcap.m_sellTimeChkBaseSec = 0;             // +0x3c（0x8e42c）
    user->m_mcap.m_timeChkCount = 0;                   // +0x50（0x8e440）

    PacketGuard auctionGuard;                     // -0xb8
    auctionGuard.put_header(0, 0xb7);
    auctionGuard.put_byte(0);
    if (GlobalData::s_auction_proxy->IsRunning())
        auctionGuard.put_byte(1);
    else
        auctionGuard.put_byte(0);
    auctionGuard.finalize(true);
    user->Send(auctionGuard);

    PacketGuard ceraAuctionGuard;                 // -0xe8
    ceraAuctionGuard.put_header(0, 0xb7);
    ceraAuctionGuard.put_byte(1);
    if (GlobalData::s_cera_auction_proxy->IsRunning())
    {
        ceraAuctionGuard.put_byte(1);
        puts("\n\nInter_AuctionNotifyAuctionService::dispatch_sig : point opened\n");
    }
    else
    {
        ceraAuctionGuard.put_byte(0);
        puts("\n\nInter_AuctionNotifyAuctionService::dispatch_sig : point closed\n");
    }
    ceraAuctionGuard.finalize(true);
    user->Send(ceraAuctionGuard);

    cMyTrace loginTrace(__PRETTY_FUNCTION__, 0x6a7b, 0);
    loginTrace("USER LOGINED! SEND ENUM_NOTIPACKET_AUCTION_NOTIFY_AUCTION_SERVICE");

    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, streamGuard);
    return 0;
}
