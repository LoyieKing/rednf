bool Init(int argc, char** argv)
{
    // ORIG @0x82a08f4：regist_signals 失败 → cout "Fail to regist os signals,
    // Server process is terminated." → return 0（82a090c..82a0937 实证）。
    if (!regist_signals())
    {
        std::cout << "Fail to regist os signals, Server process is terminated."
                  << std::endl;
        return false;
    }

    // ORIG 82a093a..82a0973：getrlimit(RLIMIT_CORE) → rlim_cur=RLIM_INFINITY
    // → setrlimit(RLIMIT_CORE)（不检查返回值）；随后无条件
    // cout "[!] Check Args ..."（82a0976..82a0998）。
    struct rlimit rl;
    getrlimit(RLIMIT_CORE, &rl);
    rl.rlim_cur = static_cast<rlim_t>(-1);
    setrlimit(RLIMIT_CORE, &rl);

    std::cout << "[!] Check Args ..." << std::endl;
    if (!check_argv(argc, argv))
    {
        // ORIG 82a09b3..82a09dc：cout "[!] Check Args Fail" → return 0。
        std::cout << "[!] Check Args Fail" << std::endl;
        return false;
    }
    // ORIG 82a09e1..82a0a02：cout "[!] Check Args Success"。
    std::cout << "[!] Check Args Success" << std::endl;

    std::srand(static_cast<unsigned>(time(NULL)));

    // 日志初始化（ORIG 82a0a19..82a0ae5）：
    //   CLog::createInstance → cout "[!] Init Log ..." → sprintf
    //   "./log/%s/"（0x8c187b8）→ initiate(path, 0, 0)（第 2 参 ORIG 传 0，
    //   m_logFile 前缀回落 "Log"）→ cout "	- log path: " << path
    //   → cout "[!] Init Log Success"。
    CLog::createInstance();
    std::cout << "[!] Init Log ..." << std::endl;
    char logPath[0x104];
    // ORIG 0x082a0a64 调 `sprintf@plt`（无 size 参数，P171 照抄；旧实现 snprintf
    // 多压 size 实参，p167 EXTRA +1）。
    std::sprintf(logPath, "./log/%s/", G_CEnvironment()->get_file_name());
    CLog::instance()->initiate(logPath, 0, 0);
    std::cout << "	- log path: " << logPath << std::endl;
    std::cout << "[!] Init Log Success" << std::endl;

    // ORIG 82a0aea..82a0bc0：
    //   cMyTrace trace("AuthGarenaToken::Init"@0x8c1b026, 0x29c9, 9, 1, 0);
    //   trace("[!] Init Taiwan Garena Auth Key ..."@0x8c187e8);
    //   init(env+0x314 = m_serverEnvir.m_garenaPublickey.c_str())；
    //   init 返回 1（成功）→ xor 翻转 0 → je 82a0bc5（Success 分支：trace
    //   0x29d3 " %s Success" → 继续 GlobalData::Init）；init 返回 0（失败）
    //   → 82a0b57 起 Fail 分支：trace 0x29d0 " %s Fail" → jmp 82a2105
    //   （Init 返回 0，main 退出）。影子实证：publickey.pem Fail 两侧都退出，
    //   第二轮均 pid 残留 already operated。
    cMyTrace authTrace("AuthGarenaToken::Init", 0x29c9, 9, 1, 0);
    authTrace("[!] Init Taiwan Garena Auth Key ...");
    // P183：ORIG 0x082a0b36/0x082a0b57/0x082a0bc5 三处各自重算
    // `G_CEnvironment()+0x314 → c_str()`，无 garenaPubkey 具名缓存
    // （p167 MISSING 2×G_CEnvironment + 2×c_str 即此）。
    if (!Taiwan::AuthGarenaToken::init(
            G_CEnvironment()->m_serverEnvir.m_garenaPublickey.c_str()))
    {
        // ORIG 82a0b57..bb6：Fail trace 构造 (name, 0x29d0, 9, 0, 1)
        // ——bFieldC=0、bFieldD=1（+0xc=1 → bAppend/bTimestamp=1 → 尾部 CRLF）。
        cMyTrace failTrace("AuthGarenaToken::Init", 0x29d0, 9, 0, 1);
        failTrace(" %s Fail",
                  G_CEnvironment()->m_serverEnvir.m_garenaPublickey.c_str());
        return false;
    }
    cMyTrace okTrace("AuthGarenaToken::Init", 0x29d3, 9, 0, 1);
    okTrace(" %s Success",
            G_CEnvironment()->m_serverEnvir.m_garenaPublickey.c_str());
    CodePage::initCodePage();

    // ORIG 82a0bc5..：各启动阶段 cMyTrace(5-arg) 记录 + 失败即 abort（返回 0）。
    // 此前重建忽略多数返回值（带病启动），现按 ORIG 逐阶段补失败处理与 trace 日志。
    { cMyTrace t(__PRETTY_FUNCTION__, 0x29dd, 9, true, true); t("[!] Init GlobalData ..."); }
    if (!GlobalData::Init())
    {
        { cMyTrace t(__PRETTY_FUNCTION__, 0x29e0, 9, true, true); t("[!] Init GlobalData Fail"); }
        return false;
    }
    { cMyTrace t(__PRETTY_FUNCTION__, 0x29e3, 9, true, true); t("[!] Init GlobalData Success"); }

    { cMyTrace t(__PRETTY_FUNCTION__, 0x29e5, 9, true, false); t("[!] Init HistoryLog"); }
    if (!HistoryLog::Init())
    {
        { cMyTrace t(__PRETTY_FUNCTION__, 0x29e8, 9, false, true); t("Fail"); }
        return false;
    }
    { cMyTrace t(__PRETTY_FUNCTION__, 0x29eb, 9, false, true); t(" Success"); }

    { cMyTrace t(__PRETTY_FUNCTION__, 0x29f0, 9, true, true); t("[!] Init DataManager ..."); }
    // [b28d-solo-GameMain] 真语义 bug 修：ORIG 0x82a0e7b..0x82a0ed3 检查 init 返回值
    // （`xor $1,%eax; test %al; jmp 成功`），失败分支 ctor 行号 0x29f3 + 串
    // "[!] Init DataManager Fail" 后返回 false；旧形丢弃返回值直接往下走。
    if (!G_CDataManager()->init())
    {
        { cMyTrace t(__PRETTY_FUNCTION__, 0x29f3, 9, true, true);
          t("[!] Init DataManager Fail"); }
        return false;
    }
    { cMyTrace t(__PRETTY_FUNCTION__, 0x29f6, 9, true, true); t("[!] Init DataManager Success"); }

    { cMyTrace t(__PRETTY_FUNCTION__, 0x29f9, 9, true, true); t("[!] Init GameManager ..."); }
    if (!G_CGameManager()->init())
    {
        { cMyTrace t(__PRETTY_FUNCTION__, 0x29fc, 9, true, true); t("[!] Init GameManager Fail"); }
        return false;
    }
    { cMyTrace t(__PRETTY_FUNCTION__, 0x29ff, 9, true, true); t("[!] Init GameManager Success"); }

    { cMyTrace t(__PRETTY_FUNCTION__, 0x2a02, 9, true, true);
      t("[!] Init Arad GameScriptManager ..."); }
    if (!ARAD::Singleton<ARAD::ScriptManager>::Get()->LoadScript())
    {
        { cMyTrace t(__PRETTY_FUNCTION__, 0x2a05, 9, true, true);
          t("[!] Init Arad GameScriptManager Fail"); }
        return false;
    }
    { cMyTrace t(__PRETTY_FUNCTION__, 0x2a08, 9, true, true);
      t("[!] Init Arad GameScriptManager Success"); }

    G_CGameManager()->GetQuickPartySystemManager();

    { cMyTrace t(__PRETTY_FUNCTION__, 0x2a1f, 9, true, false); t("[!] Init ChannelType "); }
    if (!G_GameWorld()->InitChannelType())
    {
        { cMyTrace t(__PRETTY_FUNCTION__, 0x2a22, 9, true, true); t("[!] Init ChannelType Fail"); }
        return false;
    }
    { cMyTrace t(__PRETTY_FUNCTION__, 0x2a25, 9, false, true); t("Success"); }
    { cMyTrace t(__PRETTY_FUNCTION__, 0x2a27, 9, true, true);
      t("\t- Channel Type : %d", G_GameWorld()->GetChannelType()); }

    { cMyTrace t(__PRETTY_FUNCTION__, 0x2a29, 9, true, false); t("[!] Init tournament db "); }
    // [b28d-solo-GameMain] 真语义 bug 修：ORIG 0x82a12e2 `call G_CEnvironment;
    // add $0x98,%eax` ⇒ 实参是 **CEnvironment+0x98 = m_dbTblFile**（Init 为该类
    // friend，直访 private 成员）；旧形 get_file_name() 返回 m_serverEnvir.m_fileName
    // （+0x198 区），实参取错（p167 EXTRA get_file_name +1 / MISSING G_CEnvironment -1）。
    if (!GlobalData::initTournamentDB(G_CEnvironment()->m_dbTblFile))
    {
        // ORIG 0x082a12fb：失败分支是 cout << "[!] Init tournament db Fail" << endl
        // （p167 MISSING 2×ostream 即此），非 cMyTrace。
        std::cout << "[!] Init tournament db Fail" << std::endl;
        return false;
    }
    { cMyTrace t(__PRETTY_FUNCTION__, 0x2a33, 9, false, true); t("Success"); }

    // DB 查询计数器挂接（DBMgr@全局 s_db_mgr）
    GlobalData::s_db_mgr->SetQueryCounterPointer(
        GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_0, E_SERVER_GROUP_NONE /*推断*/)
            ->GetQueryCounter());

    // ---- 三类 ServerProxy 创建（ORIG 82a13ac..1855 结构）----
    // ORIG：GetChannelType()==7 跳过全部；IsIntegratedPvPBaseChannel() 时
    // 遍历 m_serverGroupMap（config 键 *ip_of_* / *port_of_*）每组建 proxy；
    // 否则固定 getter 单组。
    if (G_GameWorld()->GetChannelType() != 7)
    {
        if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        {
            typedef std::map<ENUM_SERVER_GROUP,
                             std::map<std::string, std::string> > GrpMap;
            // ORIG 82a15xx：单循环遍历 m_serverGroupMap，每迭代依次建 monitor/guild/
            // statistic proxy；每 createServerProxy 前重取 G_CEnvironment()->…size()
            // 作 count 参，失败 cerr + abort。idx（-0x34）随迭代自增。
            // P183：begin/end 各自重物化 `G_CEnvironment()->m_serverGroupMap`
            // （ORIG `++it` 后紧跟 `call G_CEnvironment; add $0x50; call end()`；
            // 缓存 GrpMap& 引用会缺该调用点，p167 MISSING G_CEnvironment -1）。
            int idx = 0;
            for (GrpMap::iterator it = G_CEnvironment()->m_serverGroupMap.begin();
                 it != G_CEnvironment()->m_serverGroupMap.end(); ++it, ++idx)
            {
                ENUM_SERVER_GROUP g = it->first;
                std::map<std::string, std::string>& c = it->second;

                int mTcp = atoi(GetConfigFileString(c, "tcp_port_of_monitor"));
                int mUdp = atoi(GetConfigFileString(c, "udp_port_of_monitor"));
                char* mIp = const_cast<char*>(GetConfigFileString(c, "udp_ip_of_monitor"));
                if (!GlobalData::s_monitor_proxy_mgr->createServerProxy(
                        static_cast<int>(G_CEnvironment()->m_serverGroupMap.size()),
                        idx, g, mIp, mUdp, mTcp))
                {
                    std::cerr << "[s_monitor_proxy_mgr::Init] Fail Init (monitor server proxy)"
                              << std::endl;
                    return false;
                }

                int gTcp = atoi(GetConfigFileString(c, "tcp_port_of_guild"));
                int gUdp = atoi(GetConfigFileString(c, "udp_port_of_guild"));
                char* gIp = const_cast<char*>(GetConfigFileString(c, "udp_ip_of_guild"));
                if (!GlobalData::s_guild_proxy_mgr->createServerProxy(
                        static_cast<int>(G_CEnvironment()->m_serverGroupMap.size()),
                        idx, g, gIp, gUdp, gTcp))
                {
                    std::cerr << "[s_guild_proxy_mgr::Init] Fail Init (guild server proxy)"
                              << std::endl;
                    return false;
                }

                int sUdp = atoi(GetConfigFileString(c, "udp_port_of_statistic"));
                char* sIp = const_cast<char*>(GetConfigFileString(c, "udp_ip_of_statistic"));
                if (!GlobalData::s_statistic_proxy_mgr->createServerProxy(
                        static_cast<int>(G_CEnvironment()->m_serverGroupMap.size()),
                        idx, g, sIp, sUdp, 0))
                {
                    std::cerr << "[s_statistic_proxy_mgr::Init] Fail Init (statistic server proxy)"
                              << std::endl;
                    return false;
                }
            }
        }
        else
        {
            // 固定 getter 单组（ORIG 82a16aa 非 Integrated 分支）
            // ORIG 每 createServerProxy 检查返回值，失败 cerr + abort。
            if (!GlobalData::s_monitor_proxy_mgr->createServerProxy(
                    1, 0, E_SERVER_GROUP_NONE,
                    G_CEnvironment()->get_udp_ip_monitor(),
                    G_CEnvironment()->get_tcp_port_monitor(),
                    G_CEnvironment()->get_udp_port_monitor()))
            {
                std::cerr << "[s_monitor_proxy_mgr::Init] Fail Init (monitor server proxy)"
                          << std::endl;
                return false;
            }
            if (!GlobalData::s_guild_proxy_mgr->createServerProxy(
                    1, 0, E_SERVER_GROUP_NONE,
                    G_CEnvironment()->get_udp_ip_guild(),
                    G_CEnvironment()->get_tcp_port_guild(),
                    G_CEnvironment()->get_udp_port_guild()))
            {
                std::cerr << "[s_guild_proxy_mgr::Init] Fail Init (guild server proxy)"
                          << std::endl;
                return false;
            }
            if (!GlobalData::s_statistic_proxy_mgr->createServerProxy(
                    1, 0, E_SERVER_GROUP_NONE,
                    G_CEnvironment()->get_udp_ip_statistic(), 0,
                    G_CEnvironment()->get_udp_port_statistic()))
            {
                std::cerr << "[s_statistic_proxy_mgr::Init] Fail Init (statistic server proxy)"
                          << std::endl;
                return false;
            }
        }
    }
    { cMyTrace t(__PRETTY_FUNCTION__, 0x2ac8, 9, true, false); t("[!] Init GameWorld "); }
    if (!G_GameWorld()->init())
    {
        { cMyTrace t(__PRETTY_FUNCTION__, 0x2acb, 9, true, true); t("[!] Init GameWorld Fail"); }
        return false;
    }
    { cMyTrace t(__PRETTY_FUNCTION__, 0x2ace, 9, false, true); t("Success"); }

    GlobalData::s_DailyScheduleManager->CheckAndSendToAllUserSchedule();

    // LoginState 初始化：遍历 statistic proxy（Start..End 索引区间）。
    // ORIG 0x2ad7/0x2aec：前后 cMyTrace；InitLoginState 失败(0x2ae8/0x2adf) abort。
    { cMyTrace t(__PRETTY_FUNCTION__, 0x2ad7, 9, true, false); t("[!] Init Login State "); }
    // [b28d-solo-GameMain] P222 块序：ORIG 真分支（Integrated → statistic 遍历循环）
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        // 同 guild 循环：ORIG -0x?? 独立计数器（GetNextIndex(int&) 引用出参，初值 0，
        // cond `m_count > next`）；旧形 idx 自身自增（跳过条目）。
        int statNext = 0;
        for (int idx = GlobalData::s_statistic_proxy_mgr->GetStartIndex();
             GlobalData::s_statistic_proxy_mgr->GetEndIndex() > statNext; )
        {
            // [b28d-solo-GameMain] 真语义 bug 修：ORIG 0x82a19e4..0x82a1a0d 实参为
            // `InitLoginState((ENUM_SERVER_GROUP)idx, get_gc_no_hardcode(idx))`
            // （esi=idx 作第一参数、ebx=get_gc_no_hardcode(idx) 作第二参数）；
            // 旧形取 get_gc_no_hardcode(E_SERVER_GROUP_NONE) + get_channel_no（错）。
            if (!G_CDataManager()->InitLoginState(
                    static_cast<ENUM_SERVER_GROUP>(idx),
                    G_CEnvironment()->get_gc_no_hardcode(
                        static_cast<ENUM_SERVER_GROUP>(idx))))
            {
                { cMyTrace t(__PRETTY_FUNCTION__, 0x2adf, 9, true, true);
                  t("[!] Init Login State Fail"); }
                return false;
            }
            idx = GlobalData::s_statistic_proxy_mgr->GetNextIndex(statNext);
        }
    }
    else
    {
        // ORIG 非 Integrated：单发 InitLoginState(m_serverGroup, m_gcNo)。
        if (!G_CDataManager()->InitLoginState(
                static_cast<ENUM_SERVER_GROUP>(
                    G_CEnvironment()->m_serverEnvir.m_serverGroup),
                G_CEnvironment()->m_serverEnvir.m_gcNo))
        {
            { cMyTrace t(__PRETTY_FUNCTION__, 0x2ae8, 9, true, true);
              t("[!] Init Login State Fail"); }
            return false;
        }
    }
    { cMyTrace t(__PRETTY_FUNCTION__, 0x2aec, 9, false, true); t("Success"); }

    if (IsLightServer())
    {
        G_CGameManager()->WarRoomCountAdjustByChannelInfo();
    }
    else
    {
        G_CGameManager()->WarRoomAlloc();
    }
    G_CGameManager()->allocBlueMarble();

    // ---- 事件表装载 ----
    {
        std::vector<int> eventIds;
        // ORIG 82a1bce..：Verify/Insert/LoadEventFromDB 任一失败记 cMyTrace 并 abort。
        if (!CEventTable::VerifyEventTable(eventIds))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x2b0e, 9, true, true);
            t("Fail verify Event Table");
            return false;
        }
        if (!CEventTable::InsertEventTable(eventIds))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x2b14, 9, true, true);
            t("Fail insert Event Table");
            return false;
        }
        for (int i = 0; i < CEventTable::GetEventTableCount(); ++i)
        {
            CEventBase* base = CEventTable::GetEventBase(i);
            int eventId = CEventTable::GetEventId(i);
            GlobalData::s_event_manager->AddEvent(eventId, base);
        }
        if (!GlobalData::s_event_manager->LoadEventFromDB())
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x2b25, 9, true, true);
            t("Fail Load Event From DB");
            return false;
        }
        // 触发周期事件（ORIG 82a1d56..dd8：getCurSec → GetRepeatEvent(0x3a)
        // → ev->IsEventing(0) 且 curSec>0x4cbe3f0f(约 2017 时间戳) 则
        // TriggerEventStart(0x3a, Word_Param())；单事件非小时槽循环）。
        long curSec = static_cast<long>(s_systemTime_.getCurSec());
        CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x3a);
        if (ev && ev->IsEventing(0) && curSec > 0x4cbe3f0f)
            GlobalData::s_event_manager->TriggerEventStart(0x3a, Word_Param());
    }
    // 公会信息装载。[b28d-solo-GameMain] 真语义 bug 修 ×2（ORIG 0x82a1dd8..0x82a1ea6）：
    // ①-0x2c0 是**独立计数器**（GetNextIndex(int&) 引用出参，初值 0，cond `m_count
    //   > next`），旧形用 gi 自身自增（跳过条目）；
    // ②LoadGuildInfoProxyFromDB 实参 = **当前索引 gi**（ORIG `mov -0x20,%ebx` 直传），
    //   旧形传 E_SERVER_GROUP_NONE（取错实参）；
    // ③ORIG 无 gp 判空（P171 照抄），链式单表达式（P52）。
    int guildNext = 0;
    for (int gi = GlobalData::s_guild_proxy_mgr->GetStartIndex();
         GlobalData::s_guild_proxy_mgr->GetEndIndex() > guildNext; )
    {
        if (!GlobalData::s_guild_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)gi)
                 ->LoadGuildInfoProxyFromDB((ENUM_SERVER_GROUP)gi))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x2b88, 9, true, true);
            t("Fail Load Guild Info Proxy");
            return false;
        }
        gi = GlobalData::s_guild_proxy_mgr->GetNextIndex(guildNext);
    }

    // 广告 / 共享服务器消息 / 拍卖均价
    // ORIG 0x2b91：LoadADExposeInfoAtApp 失败记 cMyTrace 并 abort。
    if (!InGameADManagerInstance()->LoadADExposeInfoAtApp())
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x2b91, 9, true, true);
        t("Fail Load In Game Advertisement Expose Info");
        return false;
    }
    G_CGameManager()->GetSharedServerMessageManager()->LoadServerMessageFromDB();
    G_CGameManager()->GetAuctionAveragePriceManager()->LoadAveragePrice();

    np_server_xml::CServerXml xml;  // 「推断」实例化方式（ORIG 直接调用成员）
    xml.StrLoading();

    // 死亡之塔排行榜 4 表（type 1..4）懒构造 + 逐表 init（ORIG 82a1f.. 循环）；
    // registNextTimer 传 0（ORIG 实证，非 60000）。
    for (int i = 1; i < 5; ++i)
    {
        // ORIG 0x082a1f60..0x082a1f75：getRankTable(i) 返回表指针作 this 传参，
        // 再 init(i)（非静态成员，esp[0]=this、esp[4]=i）。
        WongWork::CDeathTowerRanking* rankTable =
            (WongWork::CDeathTowerRanking*)WongWork::CDeathTowerRanking::getRankTable(i);
        rankTable->init(i);
    }
    TimerDeathTowerTopRankerReload::registNextTimer(0);

    InitCipherKeyString();
    RegistServerSchedule();

    // Power War 启动信息请求 + 各定时器注册（ORIG 顺序保留）。
    // [b28d-solo-GameMain] 真语义 bug 修：ORIG 0x82a1f9f..0x82a1fb7 是**单次调用**
    // （`mov 0x940be2c,%eax; movl $0,0x4(%esp); call GetServerProxy; mov %eax,(%esp);
    // call SendReqestPowerWarStartInfo`，无循环、无判空）；旧形写成 guild 遍历循环
    // （多发 GetStartIndex/GetEndIndex/GetNextIndex 各 1，p167 EXTRA 三连即此）。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(E_SERVER_GROUP_NONE)
        ->SendReqestPowerWarStartInfo();
    Timer_SecretShopStatistic::RegistNextTimer();
    Timer_DailySchedule::RegistNextTimer();
    advancealtar::Timer_MainTick::RegistNextTimerGage();
    Timer_OneADayItemShop::RegistNextTimer();
    Timer_EventManagement::RegistNextTimer(60000);           // 「推断」间隔
    Timer_DungeonStatisticEntrance_Hour::RegistNextTimer();

    // ORIG 0x2bf6：create_thread 失败记 "Fail to Create Threads." 并 abort。
    if (!create_thread())
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x2bf6, 9, true, true);
        t("Fail to Create Threads.");
        return false;
    }
    // ORIG 82a204e：仅 master 连接器 s_db_connector 非空才 Close()。
    if (GlobalData::s_db_connector != 0)
        GlobalData::s_db_connector->Close();
    // ORIG 0x2c0e：启动成功 trace（file_name / channel_no / channel_name 序）。
    cMyTrace t(__PRETTY_FUNCTION__, 0x2c0e, 9, true, true);
    t("[!] The %s channel %d('%s') server has been started successfully.",
      G_CEnvironment()->get_file_name(), G_CEnvironment()->get_channel_no(),
      G_CEnvironment()->get_channel_name());
    return true;
