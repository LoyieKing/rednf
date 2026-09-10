void CUser::DisConnSig(DISCONN_SIG sig, bool flag, int param)
{
    // b28d-CUser p167 真缺口：ORIG 0x08648a1b 调 `Guard<Mutex>::C2EPS0_`（RAII），
    //   全函数无 Mutex::lock/unlock call；原手动 lock+3×unlock 多 4 个调用点。
    Guard<Mutex> lockGuard(&m_mutex2);   // ORIG this+0x8cfa0 实证

    // ORIG 0x08648a45..0x08648a63：持锁后立即经 Arad_DataManager::findGameScript
    // ("Arad_EventPeriodDataManager") 取周期事件管理并 delUserRewardData(this)，
    // 不受后文 socket/state 短路影响（无条件先执行）。
    {
        ARAD::Arad_DataManager* adm = ARAD::Singleton<ARAD::Arad_DataManager>::Get();
        // 2026-09-10（FL2CUser 类型轴）：ORIG 0x08648a45 段 p167 无具名 string 局部
        //   （差集只有「我们多 1×`SsC2ERKSs` 拷贝构造 + 2×`SsD2Ev`」）；形参
        //   `_ZN4ARAD16Arad_DataManager14findGameScriptESs` 为**按值 std::string**
        //   ⇒ 原码直传字面量（临时经 elide 直达形参），无 name 局部。
        ARAD::Arad_EventPeriodDataManager* epdm =
            (ARAD::Arad_EventPeriodDataManager*)adm->findGameScript(
                "Arad_EventPeriodDataManager");
        epdm->delUserRewardData(this);
    }

    int sock = m_network.m_sock;   // ORIG this+0xe0
    int chState = m_chState;       // ORIG this+0x8cfc4
    if (sock < 0 || chState == 0)
    {
        // ORIG +0x137 trace（0x911）：无 socket / 未登录态仅留痕后返回。
        cMyTrace(__PRETTY_FUNCTION__, 0x911, 5)
            ("this %x,incre_id= %d, idx= %d, m_socket<0,m_ChState==NONE , "
              "DisConnSig %s, From %d",
              this, (int)GetIncreID(), (int)GetSlotIDX(),
              // b28d-CUser p167：ORIG 0x08648ad9 直读 0x704ac（非 get_acc_id）。
              NumberToString(m_accId, 0), (int)sig);
        return;
    }

    if (IsProgLogout())
    {
        // ORIG +0x21c trace（0x917）：已在登出中。
        cMyTrace(__PRETTY_FUNCTION__, 0x917, 5)
            ("[Prog log out...]this %x,incre_id= %d, idx= %d, DisConnSig %s, From %d",
              this, (int)GetIncreID(), (int)GetSlotIDX(),
              NumberToString(m_accId, 0), (int)sig);   // ORIG 0x08648d89 直读
        return;
    }
    SetProgLogout();

    // ORIG +0x240：GetPeerIP2 后按有无 IP 打两条 trace（0x939 无 IP / 0x937 有 IP）；
    // send/recv 计数 = +0x8cf0d/+0x8cf0c 双字节、+0x8cf08（m_lastPacketID）。
    char ip[16];
    bool haveIp = m_network.GetPeerIP2(ip, 0x10) != 0;
    {
        const unsigned char* sendCnt = (const unsigned char*)m_pad8cf0c;  // +0x8cf0c
        if (!haveIp)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x939, 0)
                ("from (%d) m_id[%s] uid[%d], send(%d:%d), recv(%d), "
                  "ip(GetPeerIP:NONE), dungeon(%d)",
                  (int)sig, NumberToString(m_accId, 0), (int)m_field704a8,
                  (int)sendCnt[1], (int)sendCnt[0], m_lastPacketID,
                  getPlayingDungeonIndex());
        }
        else
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x937, 0)
                ("from (%d) m_id[%s] uid[%d], send(%d:%d), recv(%d), ip(%s), dungeon(%d)",
                  (int)sig, NumberToString(m_accId, 0), (int)m_field704a8,
                  (int)sendCnt[1], (int)sendCnt[0], m_lastPacketID, ip,
                  getPlayingDungeonIndex());
        }
    }

    // ORIG +0x2c0：sig==5 时按 accId/ip 有无重映射断线原因（0x30/0x31/0x32）。
    int reason = (int)sig;
    if (reason == 5)
    {
        // b28d-CUser p167：ORIG 0x08648e17/0x08648e3e/0x08648e65 三处均直读 0x704ac。
        if (m_accId == 0 && ip[0] != '\0')
        {
            reason = 0x30;
            flag = true;
        }
        else if (m_accId == 0 && ip[0] == '\0')
        {
            reason = 0x32;
            flag = true;
        }
        else
        {
            reason = 0x31;
            flag = true;
        }
    }
    SendClientCrashDownLogToLogDB(reason);   // ORIG T 0x0864870e

    // ORIG +0x4a5：SetCurTime 后读小时（CommonTime+0x03，布局见 CommonTime.cpp
    // 注释：+0x00 year/+0x01 month/+0x02 day/+0x03 hour）∈(0x12,0x17) 且登录时长
    // (0,0x2a31) → 组 Ting 时段统计包发统计服务器（组 0，0x12 字节）。
    CommonTime nowTime;
    nowTime.SetCurTime();
    char hour = *((char*)&nowTime + 3);   // +0x03 m_hour（CommonTime.cpp 权威布局）
    if (0x12 < hour && hour < 0x17)
    {
        Packet_User_Ting_TimeCheck_Statistic_Add ting;
        int playSec = (int)(GlobalData::s_systemTime_.getCurSec() - GetLoginTime());
        // ORIG 0x08648efb `mov 0x704ac(%eax),%eax; mov %eax,-0x9c(%ebp)`：
        //   直读成员入死局部（P171 照抄），非 get_acc_id 调用。
        unsigned int accId = m_accId;
        (void)accId;
        if (0 < playSec && playSec < 0x2a31)
        {
            CStatisticServerProxy* proxy =
                GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0);
            if (proxy != 0)
                proxy->SendPacket((char*)&ting, 0x12);
        }
    }

    if (sock == -1)
    {
        // ORIG +0x567：socket 已断 → 仅留痕。
        LogManager::logFormat(
            1, "user.cpp", __PRETTY_FUNCTION__, 0x971,
            "[CUser::DisConnSig] Already Disconnected! Socket(-1) ACCID[%s]",
            NumberToString(m_accId, 0));   // ORIG 0x08648f5e 直读
    }
    else
    {
        // ORIG +0x5b5：reason∉{6,0x30,0x31,0x32} → 先发包 199(int reason)。
        if (reason != 6 && reason != 0x30 && reason != 0x31 && reason != 0x32)
        {
            PacketGuard pkt;
            ((InterfacePacketBuf*)&pkt)->put_header(0, 199);
            ((InterfacePacketBuf*)&pkt)->put_int(reason);
            ((InterfacePacketBuf*)&pkt)->finalize(true);
            if (Send(pkt) != 1)
            {
                LogManager::logFormat(
                    1, "user.cpp", __PRETTY_FUNCTION__, 0x988,
                    "[CUser::DisConnSig] Fail to disconnect reason send: reason[%d]",
                    reason);
            }
        }
        // ORIG +0x640：ResetEpoll 失败 → errno 留痕（0x98e）。
        int err = G_EpollHandler()->ResetEpoll(sock);
        if (err != 0)
        {
            LogManager::logFormat(
                1, "user.cpp", __PRETTY_FUNCTION__, 0x98e,
                "(m_id: %s) G_EpollHandler()->ResetEpoll(pUser->get_socket(%d)) %d(%s)",
                NumberToString(get_acc_id(), 0), sock, err, strerror(err));
        }
        // ORIG +0x730：组流(<<1<<uid)（0x991）。
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x991), true);
        // b28d-CUser p167：ORIG 0x08649181/0x086491ac 显式调 CStreamGuard::operator*
        //   （deEv）再 operator<<（P132 链保留；原 `guard <<` 无 operator* call）。
        (*guard)->operator<<((int)1);
        (*guard)->operator<<((int)GetUID());
        // ORIG +0x7c9：+0x8d0df != 1（= m_guildAgitDB.m_pad2[0]，语义未建模【推断】）
        // 时补崩溃统计 + HWSpec 上报。
        if (m_guildAgitDB.m_pad2[0] != (char)1)
        {
            if (!flag)
            {
                // ORIG +0x7f0：crash 观测计数（key=lastPacketID / dungeonIdx / party）。
                // b28d-CUser p167 真缺口：ORIG 0x086491ed/0x08649213/0x0864922c/
                //   0x0864925b **四次** G_GameWorld()+getStatisticsToObserveCrash()
                //   （P157/P183 逐使用点重调，不缓存 crash 指针）。
                G_GameWorld()->getStatisticsToObserveCrash()
                    ->incUserLastCrashPacketID(m_lastPacketID);
                G_GameWorld()->getStatisticsToObserveCrash()
                    ->incUserLastCrashDungeonID(getPlayingDungeonIndex());
                G_GameWorld()->getStatisticsToObserveCrash()
                    ->incAbnormalDownUserCount();
                G_GameWorld()->getStatisticsToObserveCrash()
                    ->incUserLastCrashPlayStatus(CheckInParty() ? 1 : 0);
            }
            // ORIG +0x883：Client_Spec 统计包（header 0x9ce/0xc06）：+0xa type=1、
            // +0xb count=hwSpec[1]、+0xc errCode=param、+0x10 errLine=最后登录频道、
            // +0x12 起 memcpy hwSpec[+4]（0xc/条 × count）；SendPacket 长
            // count*0xc+0x12（布局对照 shared/packet/include 权威头）。
            char* hwSpec = getClientHWSpec();
            unsigned char hwCount = (unsigned char)hwSpec[1];
            Packet_Client_Spec_Statistic specPkt;
            char* specBase = (char*)&specPkt;
            specPkt.m_type = 1;                                       // +0x0a
            specPkt.m_count = hwCount;                                // +0x0b
            specPkt.m_errorCode = param;                              // +0x0c（movl）
            specPkt.m_errorLine = (short)GetLastLoginChannelNo();     // +0x10
            memcpy((char*)specPkt.m_specs, hwSpec + 4, hwCount * 0xc);
            int specLen = (int)(unsigned short)(hwCount * 0xc + 0x12);
            CStatisticServerProxy* proxy = GlobalData::s_statistic_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)GetServerGroup());
            if (proxy != 0)
                proxy->SendPacket(specBase, specLen);
        }
        // ORIG +0x95a：队列 1 投递（在 if(+0x8d0df) 块之后，无条件执行）。
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)1, guard);
    }
