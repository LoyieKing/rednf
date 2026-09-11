// §6.2 巨件核销：P167 139/139 [0缺0多]；gap -36→-24；残差归因 N82（详见
// docs/asm_translation_patterns.md 负例表）。真修 3 处：
// ① getpeername 目标改对齐具名 struct 成员读 sa.ip（消非对齐 punning 引发的 memcpy call；
//    ORIG 单条 mov 4B 整装 @0x084c8738）；
// ② 删多余 memset(&sa,0,sizeof(sa))（ORIG sockaddr 槽不清零）；
// ③ ipBuf 清零改显式 memset(ipBuf,0,0x4c)（复现 ORIG call memset @0x084c8500；
//    `={'\0'}` 在 4.4-O0 内联 rep stos 形不符）。
// [b28d-lane2] verdict=DIFF ndiff=637（ORIG 0x084c7e0c）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-23。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=481（ORIG 0x84c7e0c）：P226 具名 bool/char 落槽+N18a 单段 regmove+N7 间接调用寄存器。
int Inter_MonitorLoginOK::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // [G2A-PC 本轮负结论 N，ndiff=235（ratio 0.835）] 已核项：
    //   · [334:335] ORIG `movsbl %al,%eax` vs 本方 `and $0xff,%eax`（有符号 char 读 vs
    //     无符号截断）；[264:265]/[287:288]/[390:391]/[398:399]/[549:550] ORIG 单条
    //     `movzbl %al,%eax` vs 本方 6 条 `test %al,%al; je; mov $0x1,%eax; jmp; mov $0x0,%eax`
    //     （bool 物化）——两侧方向相反，是本件最大残块。
    //   · 另有栈帧 0x12c vs 0x15c（属父泳道统一裁决的共享机制，不 hack）。
    //   本件 ORIG 726 条指令（.inc 187 行、函数完整在本件），上述位点需逐段回读报告
    //   才能定位对应源码行；本轮未取得单点直证，留 N。
    Inter_MonitorLoginOKPktView* pPkt = reinterpret_cast<Inter_MonitorLoginOKPktView*>(data);
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0x206a;

    // [K3 负结论 2026-09-11] 试删 characNoMsg/channelNoMsg 两具名槽改逐处 `pPkt->m_24/m_28`
    //   重读：我方 728→734 条（+6）、hunks 89→90 ⇒ ORIG 的重读形（mov -0x30(%ebp),%eax;
    //   mov 0x24(%eax),%eax 两段）在 4.4 下比槽读更贵，已回滚，保留具名槽。
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
    // [G07 2026-09-11 · P354 反向 if-else] strict 实证：本件 ORIG 697 条**无 xor / 无 setcc**，
    //   我方唯一 `xor $0x1,%eax` 出现在此处 —— 4.4 对 `!bool_callee()` 物化 xor
    //   （probe /tmp/g07/probe/pol.cpp：`if(!bf()){body}` ⇒ call;xor;test;je；`if(bf()){}else{body}`
    //   ⇒ call;test;jne 无 xor）。改为正向 if-else（两臂互换），保持语义与实参装载序不变。
    //   ⚠ P59：分支落点随之交换，A/B 若未改善即回滚。
    if (GlobalData::s_timeGate_->isOpen())
        packet.put_short(GlobalData::s_timeGate_->getNpcIndex());
    else
        packet.put_short(0);
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
}