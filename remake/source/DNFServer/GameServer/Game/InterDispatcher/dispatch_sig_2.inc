// GameWorld 局部视图补充本函数所需成员（签名与 GameWorld.h/ORIG mangled 一致）
// [b28d-lane2] verdict=DIFF ndiff=2300（ORIG 0x084c5294）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+66。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=1326（ORIG 0x084c5294）：N22 cmp 操作数方向+P226 具名 bool/char 落槽+N18a 单段 regmove。
// [BigParty/P331] verdict=DIFF ndiff 2352→1691、our 2396→2441（ORIG 2462，gap 66→21）：
//   已核销的缺口 = 两个处罚回放循环各删一处 `SIG_LOGIN_PUNISH_INFO& entry =
//   pInfo->m_punishInfos[i]` 起基址引用（宏化为每次直取），复现 ORIG 14 处逐次物化
//   （ORIG 0x84c662b `lea (%ecx,%eax,1),%eax; add $0x3a54,%eax; mov (%eax),%eax` 9 处、
//   `add $0x3a68` 5 处；-0x60(%ebp) 唯一写入是 `mov 0x10(%ebp),%eax` = 第 2 参数 data，
//   与本方 pInfo 同物；offsetof(SIG_LOGIN_DATA,m_punishInfos)=0x3a54、stride 0x18 与
//   `imul 3;shl 3` 一致 ⇒ 非布局错）。
//   剩余判 N4 常量折叠 + N82：同一寻址两侧折叠路径不同 ——
//   ORIG `add $0x3a54,%eax; mov (%eax),%eax`  vs  本方 `add $0x3a50,%eax; mov 0x4(%eax),%eax`；
//   ORIG `add $0x3a68,%eax; mov (%eax),%eax`  vs  本方 `add $0x3a60,%eax; mov 0x8(%eax),%eax`。
//   指令条数相同、仅立即数/位移拆分不同 ⇒ 4.4 从 m_punishInfoCount(+0x3a50) 起算，
//   ORIG 编译器从 m_punishInfos(+0x3a54) 起算，非源级可控（勿以手拆常量去凑，P321-T5）。
// [BigInterLogin/2026-09-09] 逐段语义核对（Ghidra C + ORIG 汇编 2462 条逐区段）发现并修复 4 类真缺口：
//   ① `if (m_breakAwayRewardBoundA < m_breakAwayRewardBoundB)` **方向反**（真 bug）：
//      ORIG 0x84c771d `mov 0x3f40(A),%edx; mov 0x3f44(B),%eax; cmp %eax,%edx; jbe 0x84c7736`
//      ⇒ A<=B 走 order3、A>B 走 order1（Ghidra C 同结论：0x3f44<0x3f40→order1）；
//      旧码 A<B 走 order1 ⇒ A<B 时结果与 ORIG 相反。已改 `A > B`。
//   ② SetServerGroup×2 / setOnlinePreliminaryServerGroup 三处 `(char)` 截断：
//      ORIG 0x84c52c0/0x84c52ef/0x84c6fd5 均 `mov 0x39d0|0x39a8(%eax),%eax` 直传 32 位
//      （无 movsbl；字段 unsigned int）⇒ 旧码把值截成低字节（≥0x100 时传错）。已删 cast。
//   ③ setTotalHackUserTradeGold 只传低 32 位：ORIG 0x84c68de `mov 0x3a2c(%eax),%edx;
//      mov 0x3a28(%eax),%eax; mov %eax,0x4(%esp); mov %edx,0x8(%esp)` ⇒ 传完整 64 位
//      （低@+0x3a28、高@+0x3a2c）；旧码高位恒 0（真值差）。已改 ull 视图读。
//   ④ restrictedGoods 循环变量 unsigned short ⇒ 16 位 `cmp %ax; seta`（count>0x7fff 边界异）；
//      ORIG 0x84c6419 `movzwl 0x293a; movzwl %ax,%eax; cmp i; setg`（32 位有符号）。已改 int。
//   ⑤ m_lastPlayTime 比较：ORIG 0x84c754f `cmp %eax,%ebx; setl`（T2 有符号直证）；
//      字段是 unsigned int ⇒ 直比生 setb（方向同、符号性异）。写
//      `static_cast<long>(m_lastPlayTime) + 0x3b53800` 复现 ORIG `lea`+`setl`（0 额外指令）。
//   复核：p167 385/385（差集仅 C1↔C2/D1↔D2 镜像）、gap +21→+23（删 3 movsbl +1 movzwl
//   +1 add，均为纠错副作用）、ndiff 1277→1270、TU 四数 121/1329/5/135 与改动前一致。
//   试形回退记录：stream/monStream/expire/repeatEvent 内联（P0 方向）实测帧 0x28c→0x26c、
//   ndiff 1270→1319（ORIG 0x84c616b 有 gameOption -0x54 具名槽 ⇒ 原码该处有局部；
//   槽基由聚合对象排布决定，N82/P212）⇒ 已回退。
//   剩余 N：N4（0x3a54/0x3a68 vs 0x3a50+4/0x3a60+8 折叠路径）、N82/P112（帧 0x28c vs 0x27c 槽平移）、
//   N7/N18a（寄存器中转）、P227/P328⑩（trace 两语句 vs 全表达式临时链）、
//   P168a/P194（具名 bool punishNow/lockNow/inWindow/startBreakAway 落槽）、P196 无（⑤ 已修）。
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
            cMyTrace(__PRETTY_FUNCTION__, 0x1b0a, 0)("pInfo->err_code == SIG_LOGIN_DATA::E_CANTFIND");
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
            cMyTrace(__PRETTY_FUNCTION__, 0x1b67, 0)("pInfo->err_code == SIG_LOGIN_DATA::PUNISH_REASON");
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
            cMyTrace(__PRETTY_FUNCTION__, 0x1b77, 0)("pInfo->err_code == SIG_LOGIN_DATA::E_AUTH");
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
            cMyTrace(__PRETTY_FUNCTION__, 0x1ba0, 0)("pInfo->err_code == SIG_LOGIN_DATA::E_AUTH_MOUSE");
            user->DisConnSig(DISCONN_SIG(0x1a), true, 0);
            return 0;
        }
        if (pInfo->m_errCode == 6)                       // E_AUTH_MOUSE_DENY
        {
            guard.put_byte(0);
            guard.put_byte(0x49);
            guard.finalize(true);
            user->Send(guard);
            cMyTrace(__PRETTY_FUNCTION__, 0x1baa, 0)("pInfo->err_code == SIG_LOGIN_DATA::E_AUTH_MOUSE_DENY");
            user->DisConnSig(DISCONN_SIG(0x1a), true, 0);
            return 0;
        }
        if (pInfo->m_errCode == 9)                       // E_PASSPAD_WRONG_VERSION
        {
            guard.put_byte(0);
            guard.put_byte(0xa6);
            guard.finalize(true);
            user->Send(guard);
            cMyTrace(__PRETTY_FUNCTION__, 0x1bb4, 0)("pInfo->err_code == SIG_LOGIN_DATA::E_PASSPAD_WRONG_VERSION");
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
            cMyTrace(__PRETTY_FUNCTION__, 0x1bf2, 0)("[Inter_LoginInfo::dispatch_sig] Abnormal LoginError: "
                  "SIG_LOGIN_DATA::ERROR_OK(%d)", pInfo->m_errCode);
            user->DisConnSig(DISCONN_SIG(0xe), true, 0);
            return 0;
        }

        cMyTrace(__PRETTY_FUNCTION__, 0x1bf6, 0)("[USER LOGIN] DB ID : %s (UID: %d)(SlotIDX: %d)(IncreID: %d)"
                   "(SocketID: %d)\n",
                   NumberToString(pInfo->m_accId, 0),
                   user->GetUID(),
                   // ORIG 0x84c5b0e `movswl %ax,%edi`（P321-T1 直证 signed short）：
                   // 裸 GetSlotIDX()（返回 unsigned short）生 movzwl ⇒ 与 ORIG 分叉，
                   // 改双 cast 复现单条符号扩展（同 RegistTimer.inc 先例）。
                   static_cast<int>(static_cast<short>(user->GetSlotIDX())),
                   user->GetIncreID(),
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
#define entry (pInfo->m_punishInfos[i])
        for (int i = 0; i < pInfo->m_punishInfoCount; ++i)
        {
            if (99 < entry.m_type && entry.m_type < 200)
            {
                bool active = !(entry.m_endTime < GlobalData::s_systemTime_.getCurSec() &&
                                entry.m_endTime != 0);
                if (active && entry.m_type == 0x65)
                {
                    user->setStdDropRate(entry.m_stdDropRate);
                }
            }
        }
        // 处罚信息回放 B：交易处罚类型映射
        for (int i = 0; i < pInfo->m_punishInfoCount; ++i)
        {
            bool active = !(entry.m_endTime < GlobalData::s_systemTime_.getCurSec() &&
                                entry.m_endTime != 0);
            if (!active)
            {
                continue;
            }
            switch (entry.m_type)
            {
            case 4:   user->SetTradePunishType(8);   break;
            case 0xb: user->SetTradePunishType(1);   break;
            case 0xc: user->SetTradePunishType(4);   break;
            case 0xd: user->SetTradePunishType(0x20); break;
            case 0xe: user->SetTradePunishType(2);   break;
            case 0x67:user->SetTradePunishType(0x80); break;
            default: break;
            }
        }
#undef entry

        user->setTotalHackUserTradeCnt(pInfo->m_totalHackUserTradeCnt);
        // ORIG 0x84c68de：`mov 0x3a2c(%eax),%edx; mov 0x3a28(%eax),%eax; mov
        // %eax,0x4(%esp); mov %edx,0x8(%esp)` ⇒ 传完整 64 位（低@+0x3a28、
        // 高@+0x3a2c）。只传低 32 位会丢失高位（真值差）。
        user->setTotalHackUserTradeGold(
            *reinterpret_cast<unsigned long long*>(
                &pInfo->m_totalHackUserTradeGold));
        if (GlobalData::g_autoPunishRuleBlackIPMgr->isBlackIP(
                pInfo->m_blackIpCheckBuf))
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, WongWork::ENUM_HACKTYPE(0x32a), 1, 0, 0);
        }

        user->setRestingUserRestrict(pInfo->m_restingUserRestrict != false);
        user->setHumanCertifyErrorCnt(0);
        user->setHumanCertified(true);

        time_t now = GlobalData::s_systemTime_.getCurSec();
        struct tm localNow;
        struct tm* pNow = localtime_r(&now, &localNow);
        int curYear = pNow->tm_year;
        int curMon = pNow->tm_mon;
        int curMday = pNow->tm_mday;

        user->getDungeonGainedGold()->setGainedGold(pInfo->m_dungeonGainedGold);
        user->SetReliablePerson(true);
        user->SetReliablePerson(pInfo->m_reliablePersonFlag != 0);

        // 当日 06:00 窗口外的超限金币清零
        pNow->tm_hour = 6;
        time_t dayStart = mktime(pNow);
        if (dayStart < static_cast<long>(pInfo->m_lastPlayTime))
        {
            user->SetOverTradeGoldCount(pInfo->m_overTradeGoldCount);
        }
        else
        {
            user->SetOverTradeGoldCount(0);
        }

        // 同日举报数保留，跨日清零
        struct tm lastPlayTm;
        struct tm* pLast = localtime_r(
            reinterpret_cast<time_t*>(&pInfo->m_lastPlayTime), &lastPlayTm);
        if (pLast->tm_year == curYear && pLast->tm_mon == curMon &&
            pLast->tm_mday == curMday)
        {
            user->SetReportMannerlessUserCount(pInfo->m_reportMannerlessUserCount);
        }
        else
        {
            user->SetReportMannerlessUserCount(0);
        }

        user->SetSchoolNo(pInfo->m_schoolNo);
        user->SetRating(pInfo->m_rating);
        user->SetSchoolName(pInfo->m_schoolName);
        user->SetDailyBadge(pInfo->m_dailyBadge);
        user->SetTutorialSkipable(pInfo->m_tutorialSkipable);
        CommonTime commonTime;
        commonTime.SetCurTime();
        user->SetAge(static_cast<unsigned int>(
            static_cast<int>(*reinterpret_cast<char*>(commonTime.m_pad)) -
            pInfo->m_birthYear + 0x7d1));
        user->setSecuReward(pInfo->m_secuReward != false);
        user->setSecuType(pInfo->m_secuType);
        user->setRewardType(pInfo->m_rewardType);
        user->setSecuGrade(pInfo->m_secuType & 0x27);

        if (pInfo->m_secCardActivate != false)
        {
            user->getSecurityCard()->setSecurityCardInfo(
                pInfo->m_secCardSerial, pInfo->m_secCardActivate != false,
                pInfo->m_secCardCertified != false, pInfo->m_secCardCertCnt,
                pInfo->m_secCardCertFlag, pInfo->m_secCardCancelCnt,
                pInfo->m_secCardAnswers);
        }
        user->getPad()->setVersionInfo(pInfo->m_passPadVersion);

        if (pInfo->m_passPadEnable != false)             // GoblinPad 主流程
        {
            // ORIG:每处使用重新调用 getPad()（无缓存）。
            user->getPad()->setActivate(true);
            user->getPad()->setCancelCnt(pInfo->m_passPadCancelCnt);
            user->getPad()->setVersionInfo(pInfo->m_passPadVersion);
            user->getPad()->setCertified(pInfo->m_passPadCertified != false);
            user->getPad()->setCheckTime(pInfo->m_passPadCheckTime);
            bool inWindow = CheckDailyScheduleTime(
                G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                pInfo->m_lastPlayTime,
                GlobalData::s_systemTime_.getCurSec());
            if (!inWindow)
            {
                user->getPad()->setFailCnt(pInfo->m_passPadFailCnt);
            }
            else if (0 < pInfo->m_passPadFailCnt)
            {
                user->getPad()->setFailCnt(0);
                user->getPad()->setCancelCnt(0);
                DB_PassPadUpdateFailCnt::makeRequest(user->get_acc_id(),
                                              static_cast<unsigned int>(user->getPad()->getFailCnt()),
                                              const_cast<char*>(user->getWebAddress()));
                cMyTrace(__PRETTY_FUNCTION__, 0x1df5, 0)("GOBLIN PAD RESET OK!!!");
            }
            user->getPad()->setPassword(pInfo->m_passPadPassword);
            // ORIG:每次使用重新调用 getHades()。
            static_cast<XNuclear::CHades*>(user->getHades())->Send_Login();
            static_cast<XNuclear::CHades*>(user->getHades())->StdCateUpdate(3, 0);
            user->getPad()->setAdvideChangePwd(
                pInfo->m_passPadAdvideChangePwd != false);
        }
        user->getPad()->setCertified(true);

        switch (pInfo->m_punishTradeAlertType)
        {
        case 0xd: user->setPunishTradeAlert(0xe7); break;
        case 0xe: user->setPunishTradeAlert(0xe8); break;
        default:  user->setPunishTradeAlert(0);    break;
        }

        user->setOnlinePreliminaryCharacNo(pInfo->m_onlinePrelimCharacNo);
        user->setOnlinePreliminaryServerGroup(static_cast<ENUM_SERVER_GROUP>(
            pInfo->m_onlinePrelimServerGroup));
        user->setOnlinePreliminaryPlayType(pInfo->m_onlinePrelimPlayType);
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1e33, 0)("OnlinePreliminary m_id(%s), play_type(%d)",
                    NumberToString(pInfo->m_accId, 0),
                    pInfo->m_onlinePrelimPlayType);
        }
        user->setTournamentAccount(pInfo->m_tournamentAccountFlag != 0 &&
                                   pInfo->m_tournamentServerGroup != 0);

        if (G_GameWorld()->GetChannelType() == 7)       // 综合赛事渠道上报
        {
            Packet_Monitor_UDP_Login pkt;
            Stream* monStream = GlobalData::s_stream_pool->Acquire(
                "InterDispatcher.cpp", 0x1e3b);
            CStreamGuard monGuard(monStream, true);
            monGuard.operator*()->operator<<(0x41);
            monGuard.operator*()->operator<<(static_cast<int>(user->GetUID()));
            pkt.m_channelId = user->GetUID();
            pkt.m_dbid = user->get_acc_id();
            pkt.m_channel =
                static_cast<unsigned char>(G_CEnvironment()->get_channel_no());
            pkt.m_sex = 1;
            pkt.m_cache0 = 0;
            pkt.m_cache4 = 0;
            monGuard->put_binary(&pkt, sizeof(pkt));
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, monGuard);
        }

        user->setClientVersion(pInfo->m_clientVersion);
        user->SetAccountLastPlayTime(pInfo->m_lastPlayTime);
        user->setJoinedSchoolPointEvent(
            pInfo->m_joinedSchoolPointEventFlag != false);
        user->m_field8ec32 = 0;

        char* punIp = user->m_network.GetPeerIP2(user->m_mcap.m_pcRoomIp, 0x14);
        user->m_mcap.m_blackIpPunishMask =
            GlobalData::g_blackIPMonitor->getPunishType(punIp);
        if ((user->m_mcap.m_blackIpPunishMask & 1) != 0)
        {
            bool punishNow = false;
            char* maskIp = user->m_network.GetPeerIP2(user->m_mcap.m_pcRoomIp, 0x14);
            if (!(GlobalData::g_blackIPMonitor->GetEndTime(maskIp,
                      WongWork::ENUM_BLACKIPMONITOR_PUNISH_TYPE(1)) <
                  GlobalData::s_systemTime_.getCurSec()))
            {
                punishNow = true;
            }
            if (punishNow)
            {
                user->SetTradePunishType(0x10);
                DB_BlackipPrivateSecurityValuation::makeRequest(user->get_acc_id());
            }
        }

        int cleanPadBase = pInfo->m_cleanPadDecreaseCnt;
        if (!CheckDailyScheduleTime(
                G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                pInfo->m_lastPlayTime,
                GlobalData::s_systemTime_.getCurSec()))
        {
            --pInfo->m_cleanPadDecreaseCnt;
        }
        else
        {
            pInfo->m_cleanPadDecreaseCnt = 0;
        }
        user->m_mcap.incPoint(pInfo->m_cleanPadDecreaseCnt);
        if (user->m_mcap.getPoint() ==
            static_cast<unsigned int>(cleanPadBase))
        {
            user->m_mcap.disableCleanPadPoint();
        }
        if ((user->m_mcap.m_blackIpPunishMask & 2) != 0)
        {
            bool lockNow = false;
            char* maskIp = user->m_network.GetPeerIP2(user->m_mcap.m_pcRoomIp, 0x14);
            if (!(GlobalData::g_blackIPMonitor->GetEndTime(maskIp,
                      WongWork::ENUM_BLACKIPMONITOR_PUNISH_TYPE(2)) <
                  GlobalData::s_systemTime_.getCurSec()))
            {
                lockNow = true;
            }
            if (lockNow)
            {
                user->m_mcap.m_blackIpTradeLockFlag = true;
                // ORIG 0x084c7464 `call getPoint; cmp $0x251b,%eax; setbe %al`
                // = 无符号 `<= 0x251b`（轴 B 反向：旧 `< 0x251c` 产有符号 setge）。
                if ((unsigned int)user->m_mcap.getPoint() <= 0x251bu)
                {
                    user->m_mcap.incPoint(0x251c);
                }
            }
        }

        if (pInfo->m_returnUserDbFlag == false)          // 归还用户判定
        {
            if (pInfo->m_lastPlayTime == 0)
            {
                user->ResetReturnUser();
            }
            // ORIG 0x84c754f 为 `cmp %eax,%ebx; setl`（有符号）⇒ 原码按 int
            // 语义比较（我方字段 unsigned int 直比会生 setb，P196/T2 直证）。
            else if (static_cast<long>(pInfo->m_lastPlayTime) + 0x3b53800 <
                     GlobalData::s_systemTime_.getCurSec())
            {
                user->SetReturnUser();
                int expire = GlobalData::s_systemTime_.getCurSec() + 0x127500;
                user->SetReturnUserExpireTime(expire);
                user->SetReturnUserFirstUserLogin(true);
                Timer_ReturnUserTimeout::registNextTimer(
                    user->GetUID(), user->get_unique_id() & 0xffff,
                    user->GetReturnUserExpireTime());
            }
            else
            {
                user->ResetReturnUser();
            }
        }
        else
        {
            user->SetReturnUser();
            user->SetReturnUserExpireTime(pInfo->m_returnUserExpireTime);
            user->SetReturnUserFirstUserLogin(pInfo->m_returnUserFirstLogin !=
                                              false);
            Timer_ReturnUserTimeout::registNextTimer(
                user->GetUID(), user->get_unique_id() & 0xffff,
                user->GetReturnUserExpireTime());
        }

        user->resetAccountMemberBonusFatigue();
        int fatigueTime = G_CEnvironment()->m_serverEnvir.m_fatigueTime;
        if (CheckDailyScheduleTime(fatigueTime,
                pInfo->m_memberBonusSaveTime,
                GlobalData::s_systemTime_.getCurSec()))
        {
            user->EnableSaveMemberBonusFatigue();
        }
        else
        {
            user->incAccountMemberBonusFatigue(pInfo->m_memberBonusFatigueInc);
        }

        user->setUserEventCharacterFlag(pInfo->m_userEventCharacterFlag);
        user->SetMileage(pInfo->m_mileage);
        user->setCharacLinkMessageFlag(pInfo->m_characLinkMessageFlag != false);

        if (pInfo->m_breakAwayUvListFlag != false)       // 断线回归名单刷新
        {
            pInfo->m_breakAwayUvListFlag =
                GlobalData::s_BreakAwaySys->updateUvList(pInfo) ? true : false;
        }
        if (0 < pInfo->m_breakAwayLuckPoint)
        {
            user->setBreakAwayRewardData(true);
            user->setUserBreakAwayRewardLuckPoint(pInfo->m_breakAwayLuckPoint);
        }
        if (0 < pInfo->m_breakAwayDungeonClearCnt)
        {
            // ORIG:if/else 两分支各一次调用（静态双站点）。
            // ORIG 0x84c771d：`cmp %eax,%edx`(edx=A,eax=B)+`jbe` → A<=B 走
            // order3、A>B 走 order1（Ghidra 同结论：0x3f44<0x3f40→order1）。
            if (pInfo->m_breakAwayRewardBoundA > pInfo->m_breakAwayRewardBoundB)
            {
                user->setBreakAwayRewardOrder(static_cast<ENUM_BREAK_AWAY_REWARD_ORDER>(1));
            }
            else
            {
                user->setBreakAwayRewardOrder(static_cast<ENUM_BREAK_AWAY_REWARD_ORDER>(3));
            }
            user->setBreakAwayDungeonClearCnt(pInfo->m_breakAwayDungeonClearCnt);
            user->setBreakAwayQuestTime(pInfo->m_breakAwayQuestTime);
        }
        if (-1 < pInfo->m_breakAwayAccureCera)
        {
            user->setBreakAwayAccureCera(pInfo->m_breakAwayAccureCera);
        }

        CEventBase* repeatEvent =
            GlobalData::s_event_manager->GetRepeatEvent(0x34);
        bool startBreakAway =
            repeatEvent->IsEventing(0) && pInfo->m_breakAwayUvListFlag != false;
        if (startBreakAway)
        {
            GlobalData::s_BreakAwaySys->startBreakAwayPreventSystem(pInfo, user);
        }

        DB_LoadAccountCargo::makeRequest(user->GetUID(), user->get_acc_id());
        user->SetPuUser(pInfo->m_puUser != false);
        user->setCharacSlotCountInfo(pInfo->m_characSlotCurCount,
                                     pInfo->m_characSlotMaxCount);
        user->set_ontime_last_recv_idx(pInfo->m_ontimeLastRecvIdx);
        user->setWebAddress(pInfo->m_webAddress);
        if (pInfo->m_checkPickUpRandomOptionItemFlag != false)
        {
            user->setCheckPickUpRandomOptionItem(true);
        }
        user->reset_server_fatigue_data();
        user->set_fatigue_count_db(pInfo->m_fatigueKind, pInfo->m_fatigueValue);
        user->set_ingame_event_history(&pInfo->m_ingameEventHistory);
        if (pInfo->m_ingameHackItemFlag != false)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, WongWork::ENUM_HACKTYPE(0x261), 1, 0, 0);
        }
        user->getAcccountHackInfo()->setLoginTime();
        user->loadInformNoticeFlag(pInfo->m_informNoticeBuf);
        user->ValidateStrikerInfo();
        user->CalcurateManageLevel();
        user->CalcurateUserMaxLevel();
        user->SetBingoData(*pInfo);
        user->initBingoData();
        user->sendReturnUserFirstLogin();
        user->SetGrowthWeaponEventInfo(pInfo->m_growthWeaponEvent);
        user->SetGrowthCreatureEventInfo(pInfo->m_growthCreatureEvent);
        user->setCharacLevelUpGift_AccountOnce(
            pInfo->m_levelupGiftAccountOnce);
        user->sendEventInfo();
        user->SetEventCreateDnfReward(false);
        DBSelectCreateDnfUserInfo::makeRequest(user->GetUID(), user->get_acc_id());
        user->m_field8ec32 = 0;

        if (Taiwan::GarenaCyberCafe(
                reinterpret_cast<Taiwan::GarenaAuthData*>(
                    user->getGarenaAuthData())->getGcaType()))
        {
            DBSelectPcroomDailyReward::makeRequest(user);
        }
    return 0;
}