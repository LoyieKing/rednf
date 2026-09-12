// [b28d-18 N89] 超大件：残差=STL 实例展开差（N89）；§6.2 语义完成度按调用差集+gap 计 | ORIG 0x08299fa0
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（大件 3139 insn）；N89（ORIG out-of-line STL
// 实例 vs 4.4 全内联）+ N82 槽位；语义完成，byte 层 N 封顶。
// [b28d-BigGlobalInit] 真缺口修复（gap +16→0，3139=3139，四数 894/1781/8/52 零回退）：
//   ① CStatisticMgr 两处 new(nothrow) 缺 value-init 前置清零 —— 删用户默认 ctor
//      （ORIG nm `W _ZN13CStatisticMgrC1Ev` 0x45 = 隐式合成）⇒ ORIG 0x829c5bc 的
//      `rep stos 0xc dwords` 复现（P299/CWG434）；
//   ② antibot init 调用缺 vptr 两级解引用 + 第二实参两段装载 —— 改
//      `(*(AntibotInitFn**)A)[0](A,(int)B,seq)`（P265/P211 档②：int 形参触发 4.4 中转）；
//   ③ CFatigueBatteryHandle 的臆造 `char m_pad[8]` 使 nothrow new 分配 8B 而 ORIG 为 1B
//      （0x829b033 `movl $0x1,(%esp)`）—— 删 pad，空类 sizeof=1（头改动已全仓广播+6 TU 复编）。
//   残余 = 1 条 ORIG 0x829cd78 `jmp`（ORIG 失败块无任何入口=死块，4.4 生成 `je 成功`，d=0 等长）
//   + N82/P112 槽位差（帧 0x79c vs 0x78c）；p167 CLEAR（565=565，差集全 C1/C2 同址别名）。
// [G7-sub 2026-09-11 判词] GlobalData::Init（ORIG 0x08299fa0，3139 条巨件）。**巨件按
// guide §6.2 三件套验收，勿以 ndiff 论成败**；结论：**封顶，无改动**。证据：
//   ① gap = 0：私有 .o（同旗标/同路由 dnf_game_cxx.sh）实测 **ORIG 3139 / 我方 3139**；
//      两侧帧完全一致 `sub $0x79c,%esp`（0x08299fa6 vs 我方对应 prologue）。
//   ② p167 调用点差集：ORIG/我方各 565 call，差异**仅** C1↔C2 / D1↔D2 **同址别名**
//      （P93；ORIG `nm` 实测 `cMyTrace::C1==C2==0x0854f746`、`PacketGuard::D1==D2==0x0858de80`、
//      `CStreamGuard::D1==D2==0x0861c8d2`）：最大项 `-39 cMyTrace::C2` 全部为此类，
//      无真缺口（0 missing / 0 extra 语义项）。
//   ③ 逐块：助记符序列仅 **1 处**差异 —— 0x0829cd78 段 ORIG 为
//      `call *%ebx; test %eax,%eax; setne %al; test %al,%al; jmp 0829cdc0`
//      （其后的 "AntiBot init fail" cMyTrace 块 0x0829cd7a..0x0829cdbe **无任何入边**
//      = ORIG 侧不可达死块；P337 家族，可用恒 0 `volatile bool` 守卫强制发射复现，本轮未做）；
//      我方同行发 `je`（该块可达）。其余为 N82/N126 槽平移（-0x35c vs -0x2bc 等）+
//      N89（4.1 对本 TU 用到的 vector/map operator[]/size 发 out-of-line 静态实例）。
// [K8GlobalData 2026-09-11 · §6.5 串盲区 + P245/P281 反馈] ORIG prett 实串
// `static bool GlobalData::Init()`（符号 _ZZN10GlobalData4InitEvE19__PRETTY_FUNCTION__）
// + ORIG 0x0829fa0 段全程无 `mov 0x8(%ebp)` 取 this ⇒ ORIG 侧是类静态成员。
// **父泳道裁决：本轮不落（N）** —— namespace 形态固有（类化重构超范围；namespace
// 内 static 变 _ZL… 内部链接反而制造符号差）⇒ prett 串差 `static` 字样记 N；
// 3139/3140 的槽位/尾岛残差维持原 N89/N126 结论。
bool GlobalData::Init()
{
    s_server_stop = false;
    s_systemTime_.update();

    s_itemAmplifier_ = new CItemAmplifier;
    s_globalEffectManager_ = new CGlobalEffectManager;

    s_timeGate_ = new (std::nothrow) CTimeGate;
    if (!s_timeGate_)
    {
        std::cerr << "[GlobalData::Init] Can't allocate s_timeGate_ memory" << std::endl;
        return false;
    }

    s_serverStateManager_ = new ARAD::Arad_ServerStateManager;

    s_msgq_mgr = new MsgQueueMgr;
    if (!s_msgq_mgr)
    {
        std::cerr << "[GlobalData::Init] can't allocate msg queue memory" << std::endl;
        return false;
    }

    s_stream_pool = new StreamPool;
    if (!s_stream_pool)
    {
        std::cerr << "[GlobalData::Init] can't allocate stream pool memory" << std::endl;
        return false;
    }

    for (int i = 0; i < 9; ++i)
    {
        s_packet_pool_list[i] = new PacketPool;
        if (!s_packet_pool_list[i])
        {
            std::cerr << "[GlobalData::Init] can't allocate packet pool memory" << std::endl;
            return false;
        }
    }
    setCurrentPacketPool(THREAD_TYPE_ACCEPT, pthread_self());

    s_small_stream_pool = new SmallStreamPool;
    if (!s_small_stream_pool)
    {
        std::cerr << "[GlobalData::Init] can't allocate small stream pool memory" << std::endl;
        return false;
    }

    s_big_stream_pool = new BigStreamPool;
    if (!s_big_stream_pool)
    {
        std::cerr << "[GlobalData::Init] can't allocate big stream pool memory" << std::endl;
        return false;
    }

    s_db_mgr = new DBMgr;
    if (!s_db_mgr)
    {
        std::cerr << "[GlobalData::Init] can't allocate db manager memory" << std::endl;
        return false;
    }

    s_db_thr = new DBThread;
    if (!s_db_thr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate db thread memory" << std::endl;
        return false;
    }
    s_db_thr->setLogDB(false);

    s_db_thr_for_logDB = new DBThread;
    if (!s_db_thr_for_logDB)
    {
        std::cerr << "[GlobalData::Init] Can't allocate db thread memory, s_db_thr_for_logDB"
                  << std::endl;
        return false;
    }
    s_db_thr_for_logDB->setLogDB(true);

    s_dispatch_thr = new DispatchThread;
    if (!s_dispatch_thr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate dispath thread memory" << std::endl;
        return false;
    }

    s_net_thr = new NetworkThread;
    if (!s_net_thr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate network thread memory" << std::endl;
        return false;
    }

    s_udp_thr = new UdpThread;
    if (!s_udp_thr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate udp thread memory" << std::endl;
        return false;
    }

    s_monitor_tcp_thr = new (std::nothrow) MonitorTcpThread;
    if (!s_monitor_tcp_thr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate monitor tcp thread memory" << std::endl;
        return false;
    }

    s_monitor_proxy_mgr = new (std::nothrow) CServerProxyMgr<CMonitorServerProxy>;
    if (!s_monitor_proxy_mgr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate monitor server proxy memory" << std::endl;
        return false;
    }

    s_guild_proxy_mgr = new (std::nothrow) CServerProxyMgr<CGuildServerProxy>;
    if (!s_guild_proxy_mgr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate guild server proxy memory" << std::endl;
        return false;
    }

    s_statistic_proxy_mgr = new (std::nothrow) CServerProxyMgr<CStatisticServerProxy>;
    if (!s_statistic_proxy_mgr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate statistic server proxy memory" << std::endl;
        return false;
    }

    // ORIG 0x0829a851-0x0829a8bc：三个实参表达式求值结果由 callee-saved 寄存器/单 spill
    // 槽（-0x77c）承载（P24/P44 内联实参即时求值；命名局部会使 4.4 三值全 spill）。
    s_hades_proxy = new (std::nothrow) CHadesServerProxy(
        G_CEnvironment()->get_udp_ip_hades(),
        G_CEnvironment()->get_tcp_port_hades(),
        G_CEnvironment()->get_udp_port_hades());
    if (!s_hades_proxy)
    {
        std::cerr << "[GlobalData::Init] Can't allocate Hades server proxy memory" << std::endl;
        return false;
    }
    if (!s_hades_proxy->Init())
    {
        std::cerr << "[GlobalData::Init] Fail Init (Hades server proxy)" << std::endl;
        return false;
    }

    {
        s_double_check_proxy = new (std::nothrow)
            CDoubleConnCheckServerProxy(G_CEnvironment()->get_udp_ip_doublecheck(),
                                        G_CEnvironment()->get_udp_port_doublecheck());
        if (!s_double_check_proxy)
        {
            std::cerr << "[GlobalData::Init] Can't allocate double conn check server proxy memory"
                      << std::endl;
            return false;
        }
        if (!s_double_check_proxy->Init())
        {
            std::cerr << "[GlobalData::Init] Fail Init (double conn check server proxy)"
                      << std::endl;
            return false;
        }
    }

    if (*G_CEnvironment()->get_school_server_ip() != '\0' &&
        G_CEnvironment()->get_school_server_port() >= 1)
    {
        s_school_proxy = new (std::nothrow)
            CSchoolServerProxy(G_CEnvironment()->get_school_server_ip(),
                               G_CEnvironment()->get_school_server_port());
        if (!s_school_proxy)
        {
            std::cerr << "[GlobalData::Init] Can't allocate school server proxy memory"
                      << std::endl;
            return false;
        }
        if (!s_school_proxy->Init())
        {
            std::cerr << "[GlobalData::Init] Fail Init (school server proxy)" << std::endl;
            return false;
        }
    }

    s_pcroom_proxy = new (std::nothrow)
        CPCRoomServerProxy(G_CEnvironment()->get_pcroom_server_ip(),
                           G_CEnvironment()->get_pcroom_server_port());
    if (!s_pcroom_proxy)
    {
        std::cerr << "[GlobalData::Init] Can't allocate pcroom server proxy memory" << std::endl;
        return false;
    }

    s_community_proxy = new (std::nothrow)
        CCommunityServerProxy(G_CEnvironment()->get_community_server_ip(),
                              G_CEnvironment()->get_community_server_port());
    if (!s_community_proxy)
    {
        std::cerr << "[GlobalData::Init] Can't allocate community server proxy memory"
                  << std::endl;
        return false;
    }

    s_auction_proxy = new (std::nothrow)
        CAuctionServerProxy(G_CEnvironment()->get_auction_server_ip(),
                            G_CEnvironment()->get_auction_server_port(), "Auction Server");
    if (!s_auction_proxy)
    {
        std::cerr << "[GlobalData::Init] Can't allocate auction server proxy memory" << std::endl;
        return false;
    }

    s_cera_auction_proxy = new (std::nothrow)
        CCeraAuctionServerProxy(G_CEnvironment()->get_cera_auction_server_ip(),
                                G_CEnvironment()->get_cera_auction_server_port(),
                                "Cera Auction Server");
    if (!s_cera_auction_proxy)
    {
        std::cerr << "[GlobalData::Init] Can't allocate cera auction server proxy memory"
                  << std::endl;
        return false;
    }

    s_power_manager = new (std::nothrow) CPowerManager;
    if (!s_power_manager)
    {
        std::cerr << "[GlobalData::Init] Can't allocate power manager memory" << std::endl;
        return false;
    }
    if (!s_power_manager->Init())
    {
        std::cerr << "[GlobalData::Init] Fail Init (power manager memory)" << std::endl;
        return false;
    }
    s_power_manager->LoadPowerWarInfo();
    s_power_manager->LoadRankerInfo();

    s_fatigueBatteryHandle_ = new (std::nothrow) CFatigueBatteryHandle;
    if (!s_fatigueBatteryHandle_)
    {
        std::cerr << "[GlobalData::Init] Can't allocate Fatigue Battery Handle memory"
                  << std::endl;
        return false;
    }

    s_secret_shop = new (std::nothrow) secretshop::CSecretShop;
    if (!s_secret_shop)
    {
        std::cerr << "[GlobalData::Init] Can't allocate SecretShop Handle memory" << std::endl;
        return false;
    }

    {
        s_channel_proxy = new (std::nothrow)
            CmmChannelProxy(G_CEnvironment()->get_udp_ip_channel(),
                            G_CEnvironment()->get_udp_port_channel());
        if (!s_channel_proxy)
        {
            std::cerr << "[GlobalData::Init] Can't allocate channel server proxy memory"
                      << std::endl;
            return false;
        }
        if (!s_channel_proxy->Init())
        {
            std::cerr << "[GlobalData::Init] Fail Init (channel server proxy)" << std::endl;
            return false;
        }
    }

    s_pcryptRijndael_CharacView_ = new CRijndael;
    s_pcryptRijndael_CharacView_->Initialize("wongyaofejsvk007", 0x20,
                                             CRijndael::sm_chain0, 0x10, 0x10, 0, 0);
    s_pcryptRijndael_Password_ = new CRijndael;
    s_pcryptRijndael_Password_->Initialize("wongyaofneople00", 0x20,
                                           CRijndael::sm_chain0, 0x10, 0x10, 0, 0);
    s_pcryptRijndael_MousePassword_ = new CRijndael;
    s_pcryptRijndael_MousePassword_->Initialize("ejsvk@@key4mouse", 0x20,
                                                CRijndael::sm_chain0, 0x10, 0x10, 0, 0);

    s_pcryptTEA_MousePassword_ = new CTEA;
    // [R12-06 ④] ORIG 0x0829b475/0x0829b4e3 `movl $0x8b23368,0xc(%esp)`（0x8b23368 =
    //   _ZN4CTEA9sm_chain0E，8B 全 0）⇒ otherKey 必须是 CTEA::sm_chain0；此前传 "" 时
    //   常量池后继 7B 为 "tjsghT"，而 Initialize 内 memcpy(m_chain/m_chain2, otherKey,
    //   m_blockSize=8)（CTEA::CTEA m_blockSize=8）⇒ 加密链初值错。
    s_pcryptTEA_MousePassword_->Initialize("tjsghTlWkdajtwla", 0x10,
                                           CTEA::sm_chain0, 0, 0);
    s_pcryptTEA_SecurityCard_ = new CTEA;
    s_pcryptTEA_SecurityCard_->Initialize("ejsvk@@@secucard", 0x10,
                                          CTEA::sm_chain0, 0, 0);

    s_pLogGameChannel = new (std::nothrow) WongWork::CLogGameChannel;
    s_pLogGameChannel->StartLogTimer();

    s_pCeraShop = new (std::nothrow) WongWork::CCeraShop;
    s_pItemVendingMachine = new (std::nothrow) ItemVendingMachine;

    s_pSecuDataControl = new Secu_DataControl;
    s_pSecuProtectionField = new CSecu_ProtectionField;

    g_autoPunishRuleBlackIPMgr = new WongWork::CAutoPunishRuleBlackIPMgr;
    g_autoPunishRuleBlackIPMgr->reset();
    g_autoPunishRuleHackTypeMgr = new WongWork::CAutoPunishRuleHackTypeMgr;
    g_autoPunishRuleHackTypeMgr->reset();
    g_blackIPMonitor = new WongWork::CBlackIPMonitor;
    g_blackIPMonitor->reset();

    s_accept_thr = new AcceptThread;
    if (!s_accept_thr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate accept thread memory" << std::endl;
        return false;
    }

    s_ref_pvp_grade = new RefPvpGrade;
    if (!s_ref_pvp_grade)
    {
        std::cerr << "[GlobalData::Init] Can't allocate pvp grade ref" << std::endl;
        return false;
    }

    s_db_connector = new (std::nothrow) CDBConnector(s_db_mgr);
    if (!s_db_connector)
    {
        return false;
    }

    DBInfoMap& masterDbInfo = G_CEnvironment()->m_dbConnInfo;
    // ORIG make_pair<ENUM_DB_HANDLE_IDX,R17ENUM_SERVER_GROUP&>（0x0829b8ba）：
    // 第二参左值本体即 ENUM_SERVER_GROUP 类型局部。
    ENUM_SERVER_GROUP serverGroup = (ENUM_SERVER_GROUP)G_CEnvironment()->m_serverEnvir.m_serverGroup;

    // ORIG 0x0829b886-0x0829d41d：groupMap 为函数级作用域（正常析构在函数尾共享块
    // 0x0829d41d；其后各 new 的 cleanup 统一经 0x0829d3ff 先析构 map 再 Unwind），
    // 不使用内层 {}。
    DBGroupMap groupMap;
    for (int i = 1; i < 0xf; ++i)
    {
        // ORIG 0x0829b8f4：make_pair 第二参引用 -0x340(%ebp)=serverGroup 局部
        // （make_pair<ENUM_DB_HANDLE_IDX,R17ENUM_SERVER_GROUP&> 实证）——插入值为
        // serverGroup，非 i。key 侧 cast 为值传（__decay_and_strip）。
        groupMap.insert(std::make_pair((ENUM_DB_HANDLE_IDX)i, serverGroup));
    }
    // ORIG 0x0829b90e-0x0829b9ce：t1 构造前 4 个独立 operator[] 语句，**字段值本体**
    // 即被求值（db 地址→-0x778 槽、user 地址→edi、port→esi、host→ebx，无引用
    // 中间变量），随后 t1 ctor + op() 从槽/寄存器直读。register 局部避免 4.4 的
    // 引用槽物化（P35 同族）。
    register char* masterDb = masterDbInfo[(ENUM_DB_HANDLE_IDX)0].m_db;
    register char* masterUser = masterDbInfo[(ENUM_DB_HANDLE_IDX)0].m_user;
    register unsigned int masterPort = masterDbInfo[(ENUM_DB_HANDLE_IDX)0].m_port;
    register char* masterHost = masterDbInfo[(ENUM_DB_HANDLE_IDX)0].m_host;
    cMyTrace t1(__PRETTY_FUNCTION__, 0x1b0c, 9, true, false);
    t1("\t- DBConnector Init(masterdb %s/%d/%s/%s) ",
       masterHost, masterPort, masterUser, masterDb);
    if (!s_db_connector->Init(masterDbInfo[(ENUM_DB_HANDLE_IDX)0]))
        {
            cMyTrace t2(__PRETTY_FUNCTION__, 0x1b0f, 9, true, true);
            t2("\t- DBConnector Init Fail");
            return false;
        }
        cMyTrace t3(__PRETTY_FUNCTION__, 0x1b12, 9, false, true);
        t3("Success");
        cMyTrace t4(__PRETTY_FUNCTION__, 0x1b14, 9, true, false);
        t4("\t- DBConnector Get Connection Info ");
        if (!s_db_connector->GetConnInfo(groupMap, masterDbInfo))
        {
            cMyTrace t5(__PRETTY_FUNCTION__, 0x1b17, 9, true, true);
            t5("\t- DBConnector Get Connection Info Fail");
            return false;
        }
        cMyTrace t6(__PRETTY_FUNCTION__, 0x1b1a, 9, false, true);
        t6("Success");
        cMyTrace t7(__PRETTY_FUNCTION__, 0x1b1c, 9, true, true);
        t7("\t- Open DB ... ");
        if (!s_db_connector->Connect(DBInfoMap(masterDbInfo)))
        {
            cMyTrace t8(__PRETTY_FUNCTION__, 0x1b1f, 9, true, true);
            t8("\t- DBConnector Connect DB Fail");
            return false;
        }
        cMyTrace t9(__PRETTY_FUNCTION__, 0x1b22, 9, true, true);
        t9("\t- All DB Open Success");

    s_psimpleSSO = new WongWork::CSimpleSSO;
    s_psimpleSSO->init(s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)6, (ENUM_SERVER_GROUP)0));

    s_securityCardCenter = new WongWork::CSecurityCardCenter;
    s_securityCardCenter->init();

    // ORIG 0x0829be0e-0x0829be68：new 结果存**局部槽** -0x34(%ebp)（Ghidra local_38），
    // Initialize 失败路径 delete 该局部，**成功后**才 `s_pIPGHelper = pHelper`
    // （0x0829be63 全局写唯一出现）。直接赋全局会使初始化读回全局、全局写前移。
    WongWork::IPG::CIPGHelper* pIPGHelper = new (std::nothrow) WongWork::IPG::CIPGHelper;
    if (pIPGHelper->Initialize(1, 5000) < 0)
    {
        puts("Fail to init IPG Module");
        delete pIPGHelper;
        return false;
    }
    s_pIPGHelper = pIPGHelper;

    CHumanCertifier::init();

    s_pGMAccounts_ = new WongWork::CGMAccounts;

    {
        cMyTrace t1(__PRETTY_FUNCTION__, 0x1d4d, 9, true, false);
        t1("\t- Init Query Table ");
        if (!s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0)
                 ->GetQueryCounter()->LoadQueryTable())
        {
            cMyTrace t2(__PRETTY_FUNCTION__, 0x1d50, 9, true, true);
            t2("\t- Init Query Table Fail");
            return false;
        }
        if (!s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)4, (ENUM_SERVER_GROUP)0)
                 ->GetQueryCounter()->LoadQueryTable())
        {
            cMyTrace t3(__PRETTY_FUNCTION__, 0x1d55, 9, true, true);
            t3("\t- Init Query Table Fail");
            return false;
        }
        cMyTrace t4(__PRETTY_FUNCTION__, 0x1d58, 9, false, true);
        t4("Success");
    }

    // ORIG 0x0829c04f-0x0829c075：`movl $0x1,-0x30(%ebp)` + `movb $0x0,-0x29(%ebp)` +
    // `cmp -0x30(%ebp),%eax; sete %al` —— 比较右操作数是 int 局部（非立即数）；
    // P28 槽序即声明序：int 先声明（-0x30 深于 bool -0x29）。
    int expectedChannel = 1;
    bool syncEnable = false;
    if (G_CEnvironment()->get_channel_no() == expectedChannel)
    {
        syncEnable = true;
    }
    sync_script::GetInstanceSyncScript()->SetEnable(syncEnable);
    if (sync_script::GetInstanceSyncScript()->IsEnable())
    {
        // ORIG 0x0829c0ff-0x0829c19d：Open 实参 R2L 求值，每个 map[5] 表达式独立
        // 求值（5 次 G_CEnvironment + 5 次 operator[]，P48/O0 无 CSE）：
        // 第1次+0x2d(pass)→spill 槽、第2次+0x18(user)→edi、第3次+0x42(db)→esi、
        // 第4次+0x14(port)→ebx、第5次+0x00(host)→edx。勿缓存引用（P157）。
        if (!s_db_mgr->Open(
                (ENUM_DB_HANDLE_IDX)5,
                G_CEnvironment()->m_dbConnInfo[(ENUM_DB_HANDLE_IDX)5].m_host,
                G_CEnvironment()->m_dbConnInfo[(ENUM_DB_HANDLE_IDX)5].m_port,
                G_CEnvironment()->m_dbConnInfo[(ENUM_DB_HANDLE_IDX)5].m_db,
                G_CEnvironment()->m_dbConnInfo[(ENUM_DB_HANDLE_IDX)5].m_user,
                G_CEnvironment()->m_dbConnInfo[(ENUM_DB_HANDLE_IDX)5].m_pass,
                (ENUM_SERVER_GROUP)0))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x1d8d, 9, true, true);
            t("\t- Failed to open web DB");
            return false;
        }
        cMyTrace t(__PRETTY_FUNCTION__, 0x1d90, 9, true, true);
        t("\t- Success WEB DB OPEN");
    }

    // ORIG 0x0829c24b-0x0829c281：聚合 bool 变量在 4.1 中寄存器化（mov $1/$0,%eax
    // join 汇聚，N3 负例——O0 源码无法复现）；此处以直接短路条件消除 slot 汇聚，
    // 语义与 ORIG 一致：两表任一失败即报错返回。
    if (!(CSyncSlangFilter::GetInstance()->AddSlangListName()
          && CSyncSlangFilter::GetInstance()->AddSlangList()))
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1d9f, 9, true, true);
        t("Failed to read slang list");
        return false;
    }

    s_event_manager = new (std::nothrow) CEventManager;
    if (!s_event_manager)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1da8, 9, true, true);
        t("Can't allocate CEventManager memory");
        return false;
    }

    cMyTrace t(__PRETTY_FUNCTION__, 0x1dc6, 9, true, true);
    t("\t- Load GM List From DB");
    gmList = new (std::nothrow) Gm_List;
    if (!gmList)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1dcd, 9, true, true);
        t("\t- Can't allocate memory for a Gm_List");
        return false;
    }

    s_GM_Manager = new (std::nothrow) CGM_Manager;
    if (!s_GM_Manager)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1dd6, 9, true, true);
        t("Can't allocate memory for a CGM_Manager");
        return false;
    }

    {
        int keyLen = G_Cipher()->GetTotalKeyLength();
        unsigned char key[1024];
        for (int i = 0; i < keyLen; ++i)
        {
            // ORIG 0x0829c54e-0x0829c564：rand() 结果经 sar/shr/and/sub 带符号取模
            // 展开（P14）⇒ 源码是 `rand() % 256`，非 C 截断强转。
            key[i] = (unsigned char)(rand() % 256);
        }
        G_Cipher()->Initialize(key, keyLen);
    }

    // ORIG 0x0829c5a2-0x0829c605：value-init nothrow new（guard 内 rep stos 清零 +
    // call C2 均为 4.1 value-init 调用侧展开；4.4 对 user-provided ctor 只发 call，
    // memset 6 行为跨版本结构性差异。手写 placement 会额外产生 _ZnwjPv/_ZdlPvS_
    // 调用，call 序列偏差更大，故用 value-init 形式）。
    s_statisticMgr = new (std::nothrow) CStatisticMgr();
    if (!s_statisticMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1df7, 9, true, true);
        t("Can't allocate memory for a CStatisticMgr");
        return false;
    }

    s_BreakAwaySys = new (std::nothrow) break_away_prevent::CBreakAwayPreventSystem;
    if (!s_BreakAwaySys)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1dfe, 9, true, true);
        t("Can't allocate memory for a s_BreakAwaySys");
        return false;
    }

    s_ExpertJobMgr = new (std::nothrow) expert_job::CExpertJobMgr;
    if (!s_ExpertJobMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e05, 9, true, true);
        t("Can't allocate memory for a CExpertJobMgr");
        return false;
    }

    s_villageObjectMgr = new (std::nothrow) village_object::CVillageObjectMgr;
    if (!s_villageObjectMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e0c, 9, true, true);
        t("Can't allocate memory for a CVillageObjectMgr");
        return false;
    }

    s_villageMonsterMgr = new (std::nothrow) village_attacked::CVillageMonsterMgr;
    if (!s_villageMonsterMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e13, 9, true, true);
        t("Can't allocate memory for a CVillageMonsterMgr");
        return false;
    }

    s_rentalSystem = new (std::nothrow) InstanceRentalSystem;
    if (!s_rentalSystem)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e1b, 9, true, true);
        t("an`t allocate memory for a InstanceRentalSystem");
        return false;
    }

    s_revengeDungeonMgr = new (std::nothrow) village_attacked::CRevengeDungeon;
    if (!s_revengeDungeonMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e24, 9, true, true);
        t("Can't allocate memory for a CRevengeDungeon");
        return false;
    }

    s_DailyScheduleManager = new (std::nothrow) CDailyScheduleManager;
    if (!s_DailyScheduleManager)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e2d, 9, true, true);
        t("Can't allocate memory for a CDailyScheduleManager");
        return false;
    }

    s_onlinePreliminaryTeamMgr = new (std::nothrow) online_preliminary::COnlinePreliminaryTeamMgr;
    if (!s_onlinePreliminaryTeamMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e36, 9, true, true);
        t("Can't allocate memory for a COnlinePreliminaryTeamMgr");
        return false;
    }

    s_GameMasterMgr = new (std::nothrow) game_master::CGameMasterMgr;
    if (!s_GameMasterMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e47, 9, true, true);
        t("Can't allocate memory for a CGameMasterMgr");
        return false;
    }

    TimerUpdatePvPGrade::registNextTimer(s_systemTime_.getCurSec());
    DB_ReloadAutoPunishRuleBackIP::makeRequest();
    DB_ReloadAutoPunishRuleHackType::makeRequest();
    DB_LoadCleanPadPoint::makeRequest();
    DB_BlackIPMonitor::makeRequest();

    s_antibotBusinessImpl_ = new CBusinessImpl;
    s_antibotChecker_ = CreateAntiServerObj("./");
    if (!s_antibotChecker_)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1eb0, 5);
        t("AntiBot create fail");
        return false;
    }

    {
        // ORIG 0x0829cd18：`movl $0x0,-0x35c(%ebp)` 单 dword 清零 —— 标量初始化形态。
        char seq[4] = {'\0'};
        // ORIG 0x8471ebd：G_CEnvironment()->getProcessSequence() 真实读环境进程序列号
        sprintf(seq, "%d", G_CEnvironment()->getProcessSequence());
        // ORIG 0x0829cd6f：init 经 vtable 槽0 调用（对象首字段=表指针，表[0]=Init：
        // 0x829cd49 `mov A,%eax; mov (%eax),%eax; mov (%eax),%ebx` 两级解引用），
        // 返回值非 0 记 "AntiBot init fail" 并返 0。
        // 第二参按 32 位整型传递（ORIG 0x829cd57 `mov B,%eax; mov %eax,%edx` 两段装载，
        // 形参 int 才触发 4.4 同形；void* 形参 4.4 折单段）。
        typedef int (*AntibotInitFn)(void*, int, char*);
        // ORIG 0x0829cd6f-0x0829cdd8：调用结果 test/setne 物化（P15），表达式直放条件位，
        // 勿先存具名局部（具名 int + `!= 0` 生成 cmpl/je 无 setne）。
        (void)(*(AntibotInitFn**)s_antibotChecker_)[0](
            s_antibotChecker_, (int)s_antibotBusinessImpl_, seq);
        // [V331b06 2026-09-12 ④ 修复] ORIG 该失败块**无任何跳转入口**：0x829cd78 `eb 46`
        //   是**无条件** jmp，跳过 0x829cd7a 的 trace/return 块（全函数无 j* 指向该址；
        //   Ghidra C 亦显示虚调用返回值被丢弃）⇒ 运行期不可达。原实现写成可达条件早退
        //   （我方 0x1101b `74 46 je`）⇒ 虚调用返回非 0 时会多打 trace 并 `return false`
        //   （ORIG 忽略返回值继续初始化）。以恒 0 `volatile` 守卫保留块体与 cMyTrace
        //   调用点（维持 p167 566/566），运行期不可达（P184/P337 族）。
        //   守卫不会被优化掉：volatile 限定的自动变量读写是可观察副作用，4.4 -O0 必须
        //   逐条发射（movb $0x0,-N; movzbl -N,%eax; test %al,%al; je），条件恒假 ⇒
        //   与 ORIG 的「块在但无入口」运行期等价。
        volatile bool deadAntiBotInitFail = false;
        if (deadAntiBotInitFail)
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x1eb7, 5);
            t("AntiBot init fail");
            return false;
        }
    }

    // ORIG 0x0829cdd4-0x0829ce2b：同上 value-init 第二处。
    s_statisticMgr = new (std::nothrow) CStatisticMgr();
    if (!s_statisticMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1ebf, 9, true, true);
        t("Can't allocate memory for a CStatisticMgr");
        return false;
    }

    g_Actions = new (std::nothrow) StackableAction::Action;
    if (!g_Actions)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1ed9, 9, true, true);
        t("Can't allocate memory for a StackableAction::Action()");
        return false;
    }

    {
        // ORIG 0x0829cf56-0x0829cf70：Acquire 结果直放 guard ctor 实参（P24），
        // 不经具名局部（命名会多生 `mov %eax,-slot; mov -slot,%eax` 往返）。
        CStreamGuard guard(s_stream_pool->Acquire("App.cpp", 0x1ee0), true);
        **guard << 0x277;
        **guard << -1;
        s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }

    // P59/P16 guard 化（ORIG 0x0829cffb-0x0829d3fd）：InitCaptcha/initialize/
    // initGeolocation 均为 bool 返回 `xor $0x1; test; je body` 前置失败块；
    // Singleton::Get() 结果直放链式调用（P52），不落具名局部。
    if (!InitCaptcha())
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1eea, 9, true, true);
        t("captcha data initialization failed");
        return false;
    }

    Timer_GenerateCaptchaData::RegistNextTimer(3);
    g_ExtractionMgr = new (std::nothrow) expert_extraction::CExtractionMgr;
    if (!g_ExtractionMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1ef5, 9, true, true);
        t("Can't allocate memory for a CExtractionMng");
        return false;
    }

    g_pTeleport[0] = new (std::nothrow) NPC_Teleport;
    if (!g_pTeleport[0])
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1f02, 9, true, true);
        t("Can't allocate memory for a NPC_Teleport()");
        return false;
    }

    g_pTeleport[1] = new (std::nothrow) User_Teleport;
    if (!g_pTeleport[1])
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1f09, 9, true, true);
        t("Can't allocate memory for a User_Teleport()");
        return false;
    }

    g_randomOptionHandle = new (std::nothrow) random_option::CRandomOptionItemHandle;
    if (!g_randomOptionHandle)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1f12, 9, true, true);
        t("Can't allocate memory for a CRandomOptionItemHandle");
        return false;
    }

    if (!ARAD::Singleton<ServiceRestrictManager>::Get()->initialize())
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1f1f, 9, true, true);
        t("Can't allocate memory for a ServiceRestrictManager");
        return false;
    }
    ARAD::Singleton<ServiceRestrictManager>::Get()->savePolicy();
    ARAD::Singleton<ServiceRestrictManager>::Get()->loadPolicy();
    if (!ARAD::Singleton<RestrictGeolocation>::Get()->initGeolocation())
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1f2a, 9, true, true);
        t("Can't initialize GeoIP");
        return false;
    }
    return true;
}