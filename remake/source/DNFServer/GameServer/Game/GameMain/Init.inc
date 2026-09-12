// ---------------------------------------------------------------------------
// Init(int, char**) —— ORIG 0x82a08f4（0x181e）。忠实骨架：按反汇编
// call 序列组织主体流程；配置解析细节（GetConfigFileString 各键名、
// proxy 数量参数）为「推断」占位。
// ---------------------------------------------------------------------------
// [b28d-28] DIFF | ORIG 0x082a08f4 | P167 MISSING 2×G_CEnvironment/2×c_str/
// ostream/sprintf/cMyTrace（初始化尾部日志路径缺口）+ N82 大函数槽。
// [b28d-solo-GameMain] N82 封顶（**语义已完整**）| p167 **CLEAR 269/269**
// （0 missing / 0 extra，§6.2 三件套之②达成；残差仅 N82 大函数槽/帧）。修法：
// ①`sprintf` 照抄（P171）+ garenaPubkey 去缓存（P183）+ tournament db Fail 分支补
//   `cout << ... << endl`；
// ②**真语义 bug ×5**：a) guild 装载循环 -0x2c0 独立计数器 + LoadGuildInfoProxyFromDB
//   实参 = (ENUM_SERVER_GROUP)gi（旧形 gi 自增 + 传 E_SERVER_GROUP_NONE）；
//   b) Power War 段 ORIG 为**单次调用**（0x82a1f9f..fb7，旧形写成 guild 遍历循环）；
//   c) `G_CDataManager()->init()` 返回值被丢弃（ORIG 0x82a0e7b 检查失败 → 行号
//   0x29f3 "[!] Init DataManager Fail" + return false）；
//   d) initTournamentDB 实参 ORIG = `G_CEnvironment()+0x98`（m_dbTblFile，Init 为
//   friend 直访），旧形 get_file_name()（= m_serverEnvir.m_fileName）取错；
//   e) statistic 循环 InitLoginState 实参 ORIG = `(idx, get_gc_no_hardcode(idx))`，
//   旧形 `get_gc_no_hardcode(E_SERVER_GROUP_NONE) + get_channel_no` 取错；
//   ③两个遍历循环均改独立计数器 + 链式单表达式（P52/P183 begin/end 各自重物化）。
bool Init(int argc, char** argv)
{
    // ORIG @0x82a08f4：regist_signals 失败 → cout "Fail to regist os signals,
    // Server process is terminated." → return 0（82a090c..82a0937 实证）。
    if (!regist_signals())
    {
        // [MiscLane 2026-09-11 真 bug 修] ORIG Init 全函数 14 处 stream 引用**全为
        // `_ZSt4cerr`**（0x948b460 ×14，`nm` 实查；0x948b3c0 `_ZSt4cout` 引用 0 处）
        // ⇒ 旧文 8 处 std::cout 系输出流写错（运行期可见：本该进 stderr 的
        // 启动日志进了 stdout）。ShowLogo（本 TU 另件）ORIG 确为 cout，勿动。
        std::cerr << "Fail to regist os signals, Server process is terminated."
                  << std::endl;
        return false;
    }

    // ORIG 82a093a..82a0973：getrlimit(RLIMIT_CORE) → rlim_cur=RLIM_INFINITY
    // → setrlimit(RLIMIT_CORE)（不检查返回值）；随后无条件
    // cout "[!] Check Args ..."（82a0976..82a0998）。
    struct rlimit rl;
    // [R22a19 2026-09-13] 复刻 ORIG 死存储（P171 照抄；写后全函数零读点）：
    //   ORIG 0x082a0950 / 0x082a0973 各一条 `mov %eax,-0x44(%ebp)` —— 槽 -0x44
    //   在 ORIG 全函数仅此 2 处引用、**皆写无读**（死存储）；ORIG 不检查返回值。
    //   我方旧文丢弃两返回值 ⇒ 比 ORIG 少 2 条 store。此处仅照抄存储形态。
    int rlResult = getrlimit(RLIMIT_CORE, &rl);
    rl.rlim_cur = static_cast<rlim_t>(-1);
    rlResult = setrlimit(RLIMIT_CORE, &rl);
    (void)rlResult;

    std::cerr << "[!] Check Args ..." << std::endl;
    // [V331b15 2026-09-12] ORIG 0x82a09b1 = `eb 2e`（**无条件** jmp 0x82a09e1）；
    // 正常编译产物该处应为 `74 2e`（je）⇒ **单字节 opcode 二进制热补**（禁用参数检查）。
    // 直证：其后 FAIL 块 0x82a09b3..0x82a09dc 在 Init 全函数**零入口**
    // （扫全区间所有 jcc/jmp 目标：82a09b3/82a09ac/82a09b1 均无命中）⇒ 运行期不可达。
    // 按 Main 2026-09-12 统一口径「ORIG 热补/死块 ⇒ 按补丁后运行时可观察行为重建」
    // （同 GlobalData::Init AntiBot / AradAppInit / logFormat）：用 P337 恒 0 volatile
    // 守卫使 FAIL 块运行期不可达，同时保留 check_argvi 调用点与块内
    // cerr/return 赋值（维持 p167 调用点齐）。
    volatile bool argsFailGuard = false;
    if (!check_argv(argc, argv) && argsFailGuard)
    {
        // ORIG 82a09b3..82a09dc：cerr "[!] Check Args Fail" → return 0（补丁后不可达）。
        std::cerr << "[!] Check Args Fail" << std::endl;
        return false;
    }
    // ORIG 82a09e1..82a0a02：cout "[!] Check Args Success"。
    std::cerr << "[!] Check Args Success" << std::endl;

    std::srand(static_cast<unsigned>(time(NULL)));

    // 日志初始化（ORIG 82a0a19..82a0ae5）：
    //   CLog::createInstance → cout "[!] Init Log ..." → sprintf
    //   "./log/%s/"（0x8c187b8）→ initiate(path, 0, 0)（第 2 参 ORIG 传 0，
    //   m_logFile 前缀回落 "Log"）→ cout "	- log path: " << path
    //   → cout "[!] Init Log Success"。
    CLog::createInstance();
    std::cerr << "[!] Init Log ..." << std::endl;
    char logPath[0x104];
    // ORIG 0x082a0a64 调 `sprintf@plt`（无 size 参数，P171 照抄；旧实现 snprintf
    // 多压 size 实参，p167 EXTRA +1）。
    std::sprintf(logPath, "./log/%s/", G_CEnvironment()->get_file_name());
    CLog::instance()->initiate(logPath, 0, 0);
    std::cerr << "	- log path: " << logPath << std::endl;
    std::cerr << "[!] Init Log Success" << std::endl;

    // ORIG 82a0aea..82a0bc0：
    //   cMyTrace trace("AuthGarenaToken::Init"@0x8c1b026, 0x29c9, 9, 1, 0);
    //   trace("[!] Init Taiwan Garena Auth Key ..."@0x8c187e8);
    //   init(env+0x314 = m_serverEnvir.m_garenaPublickey.c_str())；
    //   init 返回 1（成功）→ xor 翻转 0 → je 82a0bc5（Success 分支：trace
    //   0x29d3 " %s Success" → 继续 GlobalData::Init）；init 返回 0（失败）
    //   → 82a0b57 起 Fail 分支：trace 0x29d0 " %s Fail" → jmp 82a2105
    //   （Init 返回 0，main 退出）。影子实证：publickey.pem Fail 两侧都退出，
    //   第二轮均 pid 残留 already operated。
    // [N3Free171to190·GameMain] P139（§6.5 串盲区）：ORIG 的 cMyTrace name 实参是
    // `bool Init(int, char**)` —— 即 0x8c1b026 = `_ZZ4InitiPPcE19__PRETTY_FUNCTION__`
    // 符号本体（size 0x17 = 22+1）；手写串既不发射该静态符号（nm 缺项）又发错内容。
    cMyTrace authTrace(__PRETTY_FUNCTION__, 0x29c9, 9, 1, 0);
    authTrace("[!] Init Taiwan Garena Auth Key ...");
    // P183：ORIG 0x082a0b36/0x082a0b57/0x082a0bc5 三处各自重算
    // `G_CEnvironment()+0x314 → c_str()`，无 garenaPubkey 具名缓存
    // （p167 MISSING 2×G_CEnvironment + 2×c_str 即此）。
    if (!Taiwan::AuthGarenaToken::init(
            G_CEnvironment()->m_serverEnvir.m_garenaPublickey.c_str()))
    {
        // ORIG 82a0b57..bb6：Fail trace 构造 (name, 0x29d0, 9, 0, 1)
        // ——bFieldC=0、bFieldD=1（+0xc=1 → bAppend/bTimestamp=1 → 尾部 CRLF）。
        cMyTrace failTrace(__PRETTY_FUNCTION__, 0x29d0, 9, 0, 1);   // P139：见 authTrace
        failTrace(" %s Fail",
                  G_CEnvironment()->m_serverEnvir.m_garenaPublickey.c_str());
        return false;
    }
    cMyTrace okTrace(__PRETTY_FUNCTION__, 0x29d3, 9, 0, 1);   // P139：见 authTrace
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
    // [R8a18-v8 #16/P337 2026-09-12] 二进制热补直证：ORIG 0x82a0e7b `83 f0 01`(xor $1,%eax)
    //   / 0x82a0e7e `84 c0`(test %al,%al) / **0x82a0e80 `eb 56`(无条件 jmp 0x82a0ed8)**
    //   —— 正常编译产物应为 `74 56`(je)；失败块 0x82a0e82 在 Init 全函数**零入口**
    //   （扫全区间 jcc/jmp 目标：82a0e82 仅作为指令地址出现 1 次）⇒ 运行期不可达
    //   ⇒ ORIG 热补后行为 = **不检查** `CDataManager::init()` 返回值、恒走成功路径。
    //   按 Main 2026-09-12 统一口径（同函数 argsFailGuard / AradAppInit 0x0817de32 / check_argv
    //   同族）「ORIG 热补/死块 ⇒ 复刻补丁后运行时可观察行为 + 保留调用点与块内容」：
    //   P337 恒 0 volatile 守卫使失败块运行期不可达，但 init() 调用点、行号 0x29f3 与
    //   "[!] Init DataManager Fail" 串、return false 均照常发射（p167 调用点齐）。
    //   副作用（= ORIG 的真实行为，非我方缺陷）：init 失败时不再 return false，继续后续初始化。
    volatile bool dmInitGuard = false;
    if (!G_CDataManager()->init() && dmInitGuard)
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
        std::cerr << "[!] Init tournament db Fail" << std::endl;
        return false;
    }
    { cMyTrace t(__PRETTY_FUNCTION__, 0x2a33, 9, false, true); t("Success"); }

    // DB 查询计数器挂接（DBMgr@全局 s_db_mgr）
    // [V331b15 2026-09-12 **SEMANTIC-DIFF 修复**] ORIG 0x82a1375..0x82a1392 直证
    // `movl $0x0,0x8(%esp); movl $0x2,0x4(%esp); call DBMgr::GetDBHandle` ⇒ 句柄 idx = **2**
    // （旧写 DB_HANDLE_0=0 ⇒ 计数器挂到错误的 DB 句柄）。组仍为 0（E_SERVER_GROUP_NONE）。
    GlobalData::s_db_mgr->SetQueryCounterPointer(
        GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE)
            ->GetQueryCounter());

    // ---- 三类 ServerProxy 创建（ORIG 82a13ac..1855 结构）----
    // ORIG：GetChannelType()==7 跳过全部；IsIntegratedPvPBaseChannel() 时
    // 遍历 m_serverGroupMap（config 键 *ip_of_* / *port_of_*）每组建 proxy；
    // 否则固定 getter 单组。
    // [H10回滚 0x82a13bc：`!(x == 7)` 极性包裹实测 4.4 仍生 `setne %al; je`
    //  （sete 计数 0/1 不变，1239/1225 不变）⇒ GCC 4.4 不做 `!` 的 cc 反转，
    //  该 sete/jne 形（P59/P222）在本后端封顶。]
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

                // [N3Free171to190·GameMain] 去具名局部（P0/P183）：ORIG 的三个实参
                // 全部留在寄存器（seg119 实证 %edi/%esi/%ebx），我方具名局部迫使
                // 额外落槽 + 每处 2 条 mov（-0x5c/-0x58/-0x54）。实参求值序不变
                // （cdecl 从右到左：tcp→udp→ip→g→idx→count），语义等价。
                if (!GlobalData::s_monitor_proxy_mgr->createServerProxy(
                        static_cast<int>(G_CEnvironment()->m_serverGroupMap.size()),
                        idx, g,
                        const_cast<char*>(GetConfigFileString(c, "udp_ip_of_monitor")),
                        atoi(GetConfigFileString(c, "udp_port_of_monitor")),
                        atoi(GetConfigFileString(c, "tcp_port_of_monitor"))))
                {
                    std::cerr << "[s_monitor_proxy_mgr::Init] Fail Init (monitor server proxy)"
                              << std::endl;
                    return false;
                }

                if (!GlobalData::s_guild_proxy_mgr->createServerProxy(
                        static_cast<int>(G_CEnvironment()->m_serverGroupMap.size()),
                        idx, g,
                        const_cast<char*>(GetConfigFileString(c, "udp_ip_of_guild")),
                        atoi(GetConfigFileString(c, "udp_port_of_guild")),
                        atoi(GetConfigFileString(c, "tcp_port_of_guild"))))
                {
                    std::cerr << "[s_guild_proxy_mgr::Init] Fail Init (guild server proxy)"
                              << std::endl;
                    return false;
                }

                if (!GlobalData::s_statistic_proxy_mgr->createServerProxy(
                        static_cast<int>(G_CEnvironment()->m_serverGroupMap.size()),
                        idx, g,
                        const_cast<char*>(GetConfigFileString(c, "udp_ip_of_statistic")),
                        atoi(GetConfigFileString(c, "udp_port_of_statistic")), 0))
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
            // [R11a18 ④ 2026-09-12] 第 5/6 实参序修正：ORIG 为 **(ip, udp_port, tcp_port)**，
            //   直证 monitor：0x082a16db `mov %esi,0x18(%esp)`（esi=0x082a16b2 get_tcp_port_monitor）
            //   / 0x082a16df `mov %ebx,0x14(%esp)`（ebx=0x082a16c1 get_udp_port_monitor）；
            //   guild：0x082a176d/0x082a1771；statistic：0x082a17f0 `movl $0x0,0x18(%esp)` +
            //   0x082a17f8 `mov %ebx,0x14(%esp)`（ebx=get_udp_port_statistic）。
            //   Integrated 遍历分支两侧本就一致（键串 raw 字节已核）⇒ 不动。
            if (!GlobalData::s_monitor_proxy_mgr->createServerProxy(
                    1, 0, E_SERVER_GROUP_NONE,
                    G_CEnvironment()->get_udp_ip_monitor(),
                    G_CEnvironment()->get_udp_port_monitor(),
                    G_CEnvironment()->get_tcp_port_monitor()))
            {
                std::cerr << "[s_monitor_proxy_mgr::Init] Fail Init (monitor server proxy)"
                          << std::endl;
                return false;
            }
            if (!GlobalData::s_guild_proxy_mgr->createServerProxy(
                    1, 0, E_SERVER_GROUP_NONE,
                    G_CEnvironment()->get_udp_ip_guild(),
                    G_CEnvironment()->get_udp_port_guild(),
                    G_CEnvironment()->get_tcp_port_guild()))
            {
                std::cerr << "[s_guild_proxy_mgr::Init] Fail Init (guild server proxy)"
                          << std::endl;
                return false;
            }
            if (!GlobalData::s_statistic_proxy_mgr->createServerProxy(
                    1, 0, E_SERVER_GROUP_NONE,
                    G_CEnvironment()->get_udp_ip_statistic(),
                    G_CEnvironment()->get_udp_port_statistic(), 0))
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

    // [R7b-Main ④] ORIG 0x082a1b84..0x082a1ba5：`call IsLightServer; xor $0x1,%eax;
    //   test %al,%al; je 0x082a1baa` ⇒ **!IsLightServer() 时两个调用都执行**
    //   （WarRoomCountAdjustByChannelInfo 0x082a1b98 + WarRoomAlloc 0x082a1ba5 同属一个守卫体），
    //   随后无条件 allocBlueMarble()；旧译文 `if (IsLightServer()){adjust;} else {alloc;}`
    //   既**条件取反**又把两者写成互斥分支 ⇒ 与 ORIG 语义分叉（`IsLightServer()==true` 时
    //   ORIG 两个都不做、旧译文做 adjust；==false 时 ORIG 两个都做、旧译文只做 alloc）。
    if (!IsLightServer())
    {
        G_CGameManager()->WarRoomCountAdjustByChannelInfo();
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
        // → IsEventing(0) 且 curSec>0x4cbe3f0f(0x4cbe3f0f=1287524367) 则
        // TriggerEventStart(0x3a, Word_Param())；单事件非小时槽循环）。
        // [R7a18 ⑦轮 ④ 修复] ORIG 0x082a1d6e..0x082a1d8c 逐条直证：
        //   82a1d6e call GetRepeatEvent(0x3a)   → 返回值**无判空**（直接 `mov (%eax),%edx`）
        //   82a1d85 call *%edx                  → vtable+0x34 = CEventBase::IsEventing
        //                                         @0x080c84fa（`movzbl 0x8(%eax),%eax; ret`）
        //   82a1d87 xor $0x1,%eax               → 条件对 IsEventing 返回值**取反**
        //   82a1d8a test %al,%al / je 82a1d9e   → 取反为 0 时落 false 块
        // ⇒ 原码 = `!GetRepeatEvent(0x3a)->IsEventing(0) && curSec > 0x4cbe3f0f`
        //   （旧形 `ev && ev->IsEventing(0)` 取反缺失 + 多判空守卫 = 真语义差异）。
        long curSec = static_cast<long>(GlobalData::s_systemTime_.getCurSec());
        if (!GlobalData::s_event_manager->GetRepeatEvent(0x3a)->IsEventing(0) &&
            curSec > 0x4cbe3f0f)
        {
            // ORIG 0x82a1da7/0x82a1db0 两条 `movw $0x0,-0x2cc(%ebp)`（对象落栈
            // 槽逐字段清零）+ 0x82a1dbe `mov -0x2cc(%ebp),%edx`（4 字节读传参）
            // ⇒ 具名 Word_Param 逐字段置 0；禁 `Word_Param()` 值临时（4.4 落
            // esi 并生 `mov %esi,%eax; and $0xffff,%eax; mov %eax,%esi` 提升链）。
            Word_Param wp;
            wp.m_duration = 0;
            wp.m_field2 = 0;
            GlobalData::s_event_manager->TriggerEventStart(0x3a, wp);
        }
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

    // [R3a18 2026-09-12 第三轮核验] 接收者按 ORIG 复刻：ORIG 0x82a1f4b
    //   `movl $0x93fa660,(%esp); call np_server_xml::CServerXml::StrLoading()`
    //   —— 0x93fa660 = 全局 g_ServerString_（nm B 0x93fa660 size 0xb8），
    //   旧形用栈上临时对象 ⇒ 全局字符串表从未加载（真语义差异）。
    g_ServerString_.StrLoading();

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
    // [R3a18 2026-09-12 第三轮核验] 实参按 ORIG 复刻：ORIG 0x82a1fd0
    //   `movl $0x0,(%esp); call Timer_EventManagement::RegistNextTimer(int)`
    //   ⇒ 原码传 0（旧写的 60000 系「推断」值，非 ORIG 证据，属真语义差异）。
    Timer_EventManagement::RegistNextTimer(0);
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
}