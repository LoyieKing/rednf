int Inter_MonitorLoginOK::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_MonitorLoginOKPktView* pPkt = reinterpret_cast<Inter_MonitorLoginOKPktView*>(data);
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0x206a;

    const unsigned int characNoMsg =                   // msg+0x24
        pPkt->m_24;
    const unsigned int channelNoMsg =                  // msg+0x28
        pPkt->m_28;

    user->SetLastLoginCharacNo(0);                     // 上一角色/频道重置
    user->SetLastLoginChannelNo(0);
    user->SetValidLastLoginCharac(false);

    if (G_GameWorld()->GetChannelType() == 8 &&
        characNoMsg == 0 && channelNoMsg == 0)
    {   // 战场频道异常空载追踪
        cMyTrace(__PRETTY_FUNCTION__, 0x2075, 0)("Inter_MonitorLoginOK() mid(%s), charac_no(%d), channel_no(%d)",
              NumberToString(pPkt->m_e, 0),
              (int)characNoMsg, (int)channelNoMsg);
        return 0x2076;
    }

    cMyTrace(__PRETTY_FUNCTION__, 0x207a, 0)("Inter_MonitorLoginOK() mid(%s), charac_no(%d), channel_no(%d)",
          NumberToString(pPkt->m_e, 0),
          (int)characNoMsg, (int)channelNoMsg);

    PacketGuard packet;
    packet.put_header(1, 1);

    switch (pPkt->m_23)            // errCode
    {
    case '\x02':                                       // 重复登录断开
        packet.put_byte(0);
        packet.put_byte(0x12);
        packet.finalize(true);
        user->Send(packet);
        user->DisConnSig((DISCONN_SIG)0xc, true, 0);
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2088, 0)("info.err_code == SIG_LOGIN_DATA::ERROR_ALREADY");
        }
        return 0;
    case '\x03':                                       // 封停账号断开
        packet.put_byte(0);
        packet.put_byte(0x41);
        packet.finalize(true);
        user->Send(packet);
        user->DisConnSig((DISCONN_SIG)0x15, true, 0);
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2095, 0)("info.err_code == SIG_LOGIN_DATA::ERROR_ALREADY");
        }
        return 0;
    case '\x01':                                       // 正常登录分支
        break;
    default:
        cMyTrace(__PRETTY_FUNCTION__,
            0x2209, 5)("[%s][%d]",
                __PRETTY_FUNCTION__,
                0x2209);
        return 0x220d;
    }

    packet.put_byte(1);
    const int accNameLen = strnlen(user->get_acc_name(), 0x14);
    const bool padActivate = user->getPad()->isActivate();
    packet.put_int(accNameLen);
    packet.put_str(user->get_acc_name(), accNameLen);
    packet.put_byte(padActivate ? 1 : 0);
    packet.put_byte(0);
    if (G_CEnvironment()->get_server_group() == 3)     // garena 区/其他区分支
    {
        packet.put_byte(1);
    }
    else
    {
        Taiwan::GarenaAuthData* garena =
            reinterpret_cast<Taiwan::GarenaAuthData*>(user->getGarenaAuthData());
        packet.put_byte(garena->getMobileAuth() ? 1 : 0);
    }
    packet.put_byte(user->GetAge());
    packet.put_byte(user->IsPCRoomUser() ? 1 : 0);
    packet.put_byte(G_GameWorld()->GetChannelType());
    packet.put_byte(G_CDataManager()->GetUpgradeRevisionPvPChannel());
    packet.put_byte(user->isTournamentAccount() ? 1 : 0);
    packet.put_int(user->get_acc_id());
    // P48/P0：ORIG 6 次独立 G_CEnvironment()（strlen/put_str 双臂各一，TCPPort/UDPPort 各一）。
    packet.put_int(strlen(G_CEnvironment()->m_serverEnvir.m_relayIP));
    packet.put_str(G_CEnvironment()->m_serverEnvir.m_relayIP, 0x10);
    packet.put_int(G_CEnvironment()->m_serverEnvir.m_relayTCPPort);
    packet.put_int(G_CEnvironment()->m_serverEnvir.m_relayUDPPort);
    int sex = user->getSex();
    if (sex == -1)
        sex = 1;
    packet.put_byte(sex & 0xff);
    packet.put_byte(user->GetReliablePerson());
    packet.put_byte(user->getSecurityCard()->isActivate() ? 1 : 0);
    packet.put_byte(user->getSecurityCard()->isCertified() ? 1 : 0);
    packet.put_byte(user->getPad()->getVersionInfo());
    packet.put_byte(user->getPad()->isCertified() ? 1 : 0);
    packet.put_byte(user->isRestingUser() ? 1 : 0);

    unsigned char ipBuf[0x4c];
    memset(ipBuf, 0, 0x4c);                        // ORIG call memset 0x4c（rep stos 内联形≠ORIG）
    socklen_t saLen = sizeof(sockaddr);
    struct SockAddrIp { unsigned short family; unsigned char pad[2]; unsigned int ip; unsigned char rest[10]; } sa;
    const int ret = getpeername(user->m_network.get_socket(),
                                reinterpret_cast<sockaddr*>(&sa), &saLen);
    if (ret != 0)
        return 0x2114;                                 // socket 取 IP 失败

    {
        unsigned int clientIp = 0;
        clientIp = sa.ip;                        // ORIG mov -0xbc→-0xc4 单条 4B 整装
        void* checker = GlobalData::s_antibotChecker_;          // antibiotic 类未建模——asm 桥残留（vtable+0x04 AddClient，按 §8 协议保留）
        typedef int (*AddClientFn)(void*, int, int, unsigned int,
                                   unsigned int, void*);
        AddClientFn addClient = *(AddClientFn*)((*(void***)checker)[0x04 / 4]);
        const int antibotRet =
            addClient(checker, G_CEnvironment()->getProcessSequence(), 0,   // P48 独立求值
                      (unsigned int)user->get_acc_id(),
                      user->getClientVersion(), ipBuf);
        (void)clientIp;
        if (antibotRet < 0)
        {
            cMyTrace(__PRETTY_FUNCTION__,
                0x211b, 0)("AddClient ProcessSequence : %d, qq_id : %s, ClientVersion : %d, return : %d",
              G_CEnvironment()->getProcessSequence(),   // P48 独立求值
              NumberToString((unsigned int)user->get_acc_id(), 0),
              user->getClientVersion(), antibotRet);
        }
        user->m_field8cf00 = 0;
        user->m_field4 = 0;
        user->m_field5 = 0;
        typedef int (*P2PAddClientFn)(void*, unsigned int, int, unsigned int);  // 同上残留：vtable+0x20 P2PAddClient
        P2PAddClientFn p2pAdd =
            *(P2PAddClientFn*)((*(void***)checker)[0x20 / 4]);
        const int p2pRet =
            p2pAdd(checker, (unsigned int)user->get_acc_id(), -1,
                   user->getClientVersion());
        if (p2pRet == -1)
        {
            cMyTrace(__PRETTY_FUNCTION__,
                0x212c, 0)("P2PAddClient ProcessSequence : %d, qq_id : %s, ClientVersion : %d, return : -1",
              G_CEnvironment()->getProcessSequence(),   // P48 独立求值
              NumberToString((unsigned int)user->get_acc_id(), 0),
              user->getClientVersion(), p2pRet);
        }
        user->setAntibotKey(antibotRet);
    }

    packet.put_byte(user->getUserEventCharacterFlag() ? 1 : 0);
    packet.put_byte(user->getOnlinePreliminaryPlayType());
    packet.put_byte(online_preliminary::COnlinePreliminary::GetPlayCountPerOneTeam());
    packet.put_byte(GlobalData::s_pGMAccounts_->isGM(
                        (unsigned int)user->get_acc_id()) ? 1 : 0);
    if (!GlobalData::s_timeGate_->isOpen())
        packet.put_short(0);
    else
        packet.put_short(GlobalData::s_timeGate_->getNpcIndex());
    packet.finalize(true);
    user->Send(packet);
    user->m_field796f8 = 0;                    // ORIG 0x84c8854 Send 后清零
    {   // ORIG vtable+0x2c 两参槽（IPGHelper IPGQuery）：类未建模——asm 桥残留保留，按 §8 协议待建模清理
        typedef void (*QueryFn)(void*, CUser*);
        void* helper = GlobalData::s_pIPGHelper;
        QueryFn query = *(QueryFn*)((*(void***)helper)[0x2c / 4]);
        query(helper, user);
    }
    const unsigned short uniqueId = (unsigned short)user->get_unique_id();
    // P48：ORIG 两独立 GetUID 调用点（InsertTimer 与 makeRequest 各一），禁缓存 uid。
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0, (int)user->GetUID(),
                                (TIMER_MESSAGE)0, 300, uniqueId, 0);
    _NS_NonClient_Request(user);
    user->SetLastLoginCharacNo(characNoMsg);
    user->SetLastLoginChannelNo(channelNoMsg);
    DB_LoadDnfHackLog::makeRequest((unsigned int)user->GetUID(),
                                   (unsigned int)user->get_acc_id());
    return 0;
