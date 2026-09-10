int Inter_LoginInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // [G2A-PC 本轮负结论 N，ndiff=424（ratio 0.914）] 已核项：
    //   · 符号性轴（轴 B）信号：[437:438] ORIG `movswl %ax,%edi` vs 本方 `movzwl %ax,%edi`。
    //     该形态来自包内 short 字段读入后作 int 实参；本 .inc 全文 grep 无
    //     `short`/`unsigned short`/`(short)`（唯一 short 源是聚合器视图字段类型，禁改写域）。
    //   · 其余为 `mov (%eax),%ecx` vs `mov (%eax),%eax; mov %eax,%ecx` 型 regmove 抖动、
    //     `add $0x34,%edx; mov (%edx),%edx` vs `mov (%eax),%eax; add $0x34,%eax` 型
    //     取址序平移、以及 switch 分派岛的布局差（jne/je 反转、`mov <A>` 常量槽）。
    //   本件 2450 条指令、桩体跨 .inc 与聚合器（部分拆分），未定位到单一可判定源码点，留 N。
    SIG_LOGIN_DATA* pInfo = static_cast<SIG_LOGIN_DATA*>(
        static_cast<void*>(data));

    if (G_GameWorld()->GetChannelType() == 0xe)
    {
        user->SetServerGroup(static_cast<ENUM_SERVER_GROUP>(
            pInfo->m_onlinePrelimServerGroup));
    }
    if (G_GameWorld()->GetChannelType() == 7)
    {
        user->SetServerGroup(static_cast<ENUM_SERVER_GROUP>(
            pInfo->m_tournamentServerGroup));
    }
    user->setWebAddress(pInfo->m_webAddress);

    PacketGuard guard;
    guard.put_header(1, 1);
        if (pInfo->m_errCode == 1)                       // E_CANTFIND
        {
            guard.put_byte(0);
            guard.put_byte(6);
            guard.finalize(true);
            user->Send(guard);
            cMyTrace(g_interLoginInfoSig, 0x1b0a, 0)("pInfo->err_code == SIG_LOGIN_DATA::E_CANTFIND");
            user->DisConnSig(DISCONN_SIG(0xb), true, 0);
            return 0;
        }
        if (pInfo->m_errCode == 8)                       // 双重登录互踢
        {
            guard.put_byte(0);
            guard.put_byte(0x60);
            guard.finalize(true);
            user->Send(guard);
            user->DisConnSig(DISCONN_SIG(0x21), true, 0);
            return 0;
        }
        if (9 < pInfo->m_errCode && pInfo->m_errCode < 0xd)   // PUNISH_REASON
        {
            guard.put_byte(0);
            guard.put_byte(static_cast<unsigned char>(pInfo->m_errCode - 0x26));
            guard.put_byte(pInfo->m_loginErrDetail);
            guard.finalize(true);
            user->Send(guard);
            cMyTrace(g_interLoginInfoSig, 0x1b67, 0)("pInfo->err_code == SIG_LOGIN_DATA::PUNISH_REASON");
            user->DisConnSig(DISCONN_SIG(0x1a), true, 0);
            return 0;
        }
        if (pInfo->m_errCode == 4)                       // E_AUTH
        {
            guard.put_byte(0);
            guard.put_byte(8);
            guard.put_int(pInfo->m_punishInfos[pInfo->m_punishInfoCount].m_endTime);
            guard.finalize(true);
            user->Send(guard);
            cMyTrace(g_interLoginInfoSig, 0x1b77, 0)("pInfo->err_code == SIG_LOGIN_DATA::E_AUTH");
            user->DisConnSig(DISCONN_SIG(0x1a), true, 0);
            return 0;
        }
        if (pInfo->m_errCode == 5)                       // E_AUTH_MOUSE
        {
            guard.put_byte(0);
            guard.put_byte(0x48);
            guard.put_byte(pInfo->m_passPadFailCnt);
            guard.finalize(true);
            user->Send(guard);
            cMyTrace(g_interLoginInfoSig, 0x1ba0, 0)("pInfo->err_code == SIG_LOGIN_DATA::E_AUTH_MOUSE");
            user->DisConnSig(DISCONN_SIG(0x1a), true, 0);
            return 0;
        }
        if (pInfo->m_errCode == 6)                       // E_AUTH_MOUSE_DENY
        {
            guard.put_byte(0);
            guard.put_byte(0x49);
            guard.finalize(true);
            user->Send(guard);
            cMyTrace(g_interLoginInfoSig, 0x1baa, 0)("pInfo->err_code == SIG_LOGIN_DATA::E_AUTH_MOUSE_DENY");
            user->DisConnSig(DISCONN_SIG(0x1a), true, 0);
            return 0;
        }
        if (pInfo->m_errCode == 9)                       // E_PASSPAD_WRONG_VERSION
        {
            guard.put_byte(0);
            guard.put_byte(0xa6);
            guard.finalize(true);
            user->Send(guard);
            cMyTrace(g_interLoginInfoSig, 0x1bb4, 0)("pInfo->err_code == SIG_LOGIN_DATA::E_PASSPAD_WRONG_VERSION");
            user->DisConnSig(DISCONN_SIG(0x1a), true, 0);
            return 0;
        }
        if (pInfo->m_errCode == 7)                       // 版本不符
        {
            guard.put_byte(0);
            guard.put_byte(0x5f);
            guard.finalize(true);
            user->Send(guard);
            user->DisConnSig(DISCONN_SIG(0x20), true, 0);
            return 0;
        }

        user->setHangameUser(pInfo->m_hangameUser != false);
        if (pInfo->m_errCode != 0)                       // 其余未分类错误
        {
            guard.put_byte(0);
            guard.put_byte(1);
            guard.finalize(true);
            user->Send(guard);
            cMyTrace(g_interLoginInfoSig, 0x1bf2, 0)("[Inter_LoginInfo::dispatch_sig] Abnormal LoginError: "
                  "SIG_LOGIN_DATA::ERROR_OK(%d)", pInfo->m_errCode);
            user->DisConnSig(DISCONN_SIG(0xe), true, 0);
            return 0;
        }

        cMyTrace(g_interLoginInfoSig, 0x1bf6, 0)("[USER LOGIN] DB ID : %s (UID: %d)(SlotIDX: %d)(IncreID: %d)"
                   "(SocketID: %d)\n",
                   NumberToString(pInfo->m_accId, 0),
                   user->GetUID(), user->GetSlotIDX(), user->GetIncreID(),
                   user->get_unique_id() & 0xffff);

        // 同账号重复登录拒绝
        CUser* existing = G_CGameManager()->GetUserByAccId(pInfo->m_accId);
        if (existing != 0 && existing->m_network.get_socket() != -1 &&
            existing->get_state() != 0)
        {
            existing->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(1), 0x12);
            existing->DisConnSig(DISCONN_SIG(0xc), true, 0);
            user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(1), 0x12);
            return 0x1c18;
        }

        user->set_acc_info(pInfo->m_accId, reinterpret_cast<char*>(pInfo));
        char peerIp[16] = {'\0'};
        user->m_network.GetPeerIP2(peerIp, 0x10);

        // 地区限制检测（跳过标志=1 时免检）
        // ORIG 084c5cd3..084c5f1c：两个 std::string 均为调用点临时对象，由
        // _ZNSsC1EPKcRKSaIcE 直接从 char* 构造（无 _ZNSsC1ERKSs 拷贝构造）；
        // Singleton::Get() 在两个临时对象之后调用；&& 短路使临时对象析构走
        // 标志位公共清理块（报告 C 行 2774-2819 的 bVar6/7/9/11 + bVar8）。
        if (pInfo->m_geoAllowSkipFlag != 1 &&
            ARAD::Singleton<RestrictGeolocation>::Get()->isAllow(
                std::string(user->get_acc_name()), std::string(peerIp)) != true)
        {
            guard.put_byte(0);
            guard.put_byte(0x41);
            guard.finalize(true);
            user->Send(guard);
            user->DisConnSig(DISCONN_SIG(0x15), true, 0);
        }

        // 综合 PvP 渠道的地区码记录
        if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        {
            char countryCode[16] = {'\0'};
            // ORIG 084c5f5a..084c60ad：两处 std::string 同为调用点临时对象
            // （报告 C 行 2839-2857），无中间局部 string、无 bool 中转。
            ARAD::Singleton<RestrictGeolocation>::Get()
                ->getCountryCode(countryCode, 0x10, std::string(peerIp));
            user->setAllowLocationGeoState(
                ARAD::Singleton<RestrictGeolocation>::Get()
                    ->isAllow(std::string(countryCode)));
        }

        user->set_state(static_cast<ch_state>(2));
        G_GameWorld()->InsertLoginUser(user);

        // 向 DB 线程请求游戏选项加载
        // ORIG 0x84c60f1 Acquire 结果直进 guard 实参（无 stream 槽），但
        // 0x84c616b `mov %eax,-0x54(%ebp)` 保留 GetInBuffer 结果的具名槽
        // ⇒ 实测内联 stream 后全帧槽基右移、ndiff 1277→1319（回退，P321-T5）。
        Stream* stream = GlobalData::s_stream_pool->Acquire(
            "InterDispatcher.cpp", 0x1c7e);
        CStreamGuard streamGuard(stream, true);
        streamGuard.operator*()->operator<<(0x12e);
        streamGuard.operator*()->operator<<(static_cast<int>(user->GetUID()));
        SIG_REQ_LOAD_GAME_OPTION* gameOption =
            streamGuard->GetInBuffer<SIG_REQ_LOAD_GAME_OPTION>();
        gameOption->m_accId = user->get_acc_id();
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, streamGuard);

        G_CGameManager()->insertUserByAccID(user);
        user->SetUserDetailInfo(pInfo->m_userDetailIpPort, pInfo->m_birth,
                                pInfo->m_userDetailSsn);
        user->SetBirth(pInfo->m_birth);

        // premium 生效表回放（末项触发重算）
        for (int i = 0; i < pInfo->m_premiumCount; ++i)
        {
            bool last = (pInfo->m_premiumCount - 1 == i);
            WongWork::CHandlePremium::handleSetUserPremium(
                user, static_cast<ENUM_PREMIUM_TYPE>(pInfo->m_premiums[i].m_type),
                pInfo->m_premiums[i].m_startTime,
                pInfo->m_premiums[i].m_endTime,
                pInfo->m_premiums[i].m_flag, last);
        }

        user->setRegDate(pInfo->m_regDate);
        user->setRegTime(pInfo->m_regTime);
        if (0 < pInfo->m_characCount)
        {
            user->SetCharacInfo(pInfo->m_characCount, pInfo->m_characData);
            user->SetNeedCheckOverEquipItem();
        }

        // 雇佣兵残留竞技态 → 累积服务器侧 hack 计数
        if (0 < pInfo->m_mercenaryCount)
        {
            user->SetCharacMercenaryInfo(pInfo->m_mercenaryCount,
                                         &pInfo->m_mercenaries[0]);
            bool hasCompetition = true;
            for (int i = 0; i < pInfo->m_mercenaryCount && i < 0x24; ++i)
            {
                if (pInfo->m_mercenaries[i].m_field28 != 0)
                {
                    hasCompetition = false;
                    break;
                }
            }
            if (hasCompetition)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, WongWork::ENUM_HACKTYPE(0x1aa), 1, 0, 0);
            }
        }

        for (int i = 0; i < pInfo->m_restrictedGoodsCount; ++i)
        {
            user->SetRestrictedGoods(pInfo->m_restrictedGoods[i]);
        }

        const char* ssn = user->GetSsnString();
        unsigned char sex = user->getSex();
        char* ipRet = user->m_network.GetPeerIP2(peerIp, 0x10);
        int channelNo = G_CEnvironment()->get_channel_no();
        CMonitorServerProxy* monitorProxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                static_cast<ENUM_SERVER_GROUP>(user->GetServerGroup()));
        monitorProxy->SendLogin(user->GetUID(), pInfo->m_accId, channelNo,
                                ipRet, sex, const_cast<char*>(ssn));
        ssn = user->GetSsnString();
        sex = user->getSex();
        ipRet = user->m_network.GetPeerIP2(peerIp, 0x10);
        channelNo = G_CEnvironment()->get_channel_no();
        CGuildServerProxy* guildProxy =
            GlobalData::s_guild_proxy_mgr->GetServerProxy(
                static_cast<ENUM_SERVER_GROUP>(user->GetServerGroup()));
        guildProxy->SendLogin(user->GetUID(), pInfo->m_accId, channelNo,
                              ipRet, sex, const_cast<char*>(ssn));
        GlobalData::s_double_check_proxy->SendLogin(
            pInfo->m_accId, G_CEnvironment()->get_channel_no(),
            G_CEnvironment()->m_serverEnvir.m_serverGroup);

        user->GetGoldControl()->SetInfo(pInfo->m_goldInfo);

        // 处罚信息回放 A：有效期内的道具掉率提升（type==0x65）。
        // ORIG:每处使用重新调用 getCurSec()（无缓存）。
// §P183/P331：ORIG 对 data + i*0x18 + 0x3a54 的每个字段访问都从包基址重新物化
// （`mov -0x60(%ebp),%ecx; imul 3; shl 3; lea; add $0x3a54; mov` 共 14 处，其中
// add $0x3a54×9、add $0x3a68×5），我方原以 `SIG_LOGIN_PUNISH_INFO& entry` 起基址
// 一次 ⇒ 缺 66 条地址计算（orig 2462 / our 2396）。引用改宏直取，两循环的 i 均为
// 循环变量且循环体不改 i/pInfo ⇒ 寻址结果逐次相同，无可观察行为变化。
