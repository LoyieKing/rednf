// [S414_DB_Login] verdict=DIFF ndiff=10（17→10）：`bool execOk;` 提前到 db 之前
// 声明（赋值仍在 db->exec 处）⇒ 4.4 槽位翻正（db -0x1c→-0x18、execOk -0x15→-0x19，
// ORIG 槽 -0x18/-0x19）。余 5 行 = N1 `(i+0x866)*8+8` 折 `(i+0x867)*8`
// （P205 显式拆形/探针 probe_gift f5、probe5 六形均折）⇒ 维持封顶。
// 前史：P0/P183 具名 char* 行指针 + P232 死 int 局部修复 37→17。
bool DB_Login::GetGrowthCreatureEvent(SIG_LOGIN_DATA* sig)
{
    // [b28d-solo-DB_Login] P0/P140：ORIG 无 evt 具名局部、无判空（GetRepeatEvent
    // 返回值直传虚调用）；P5 真虚调用原地展开。
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x67)->IsEventing(0))
    {
        return 1;
    }
    bool execOk;                             // [solo 试验] 先声明，翻转 4.4 槽分配序
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect server_id, charac_no, job_type, charac_name from event_1208_growthcreature where m_id = %d",
        sig->m_accId);
    // P158/P168a④：ORIG `mov %al,-0x19; movzbl; xor $1; test; je` ⇒ bool 具名局部。
    execOk = db->exec(true);
    if (!execOk)
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__, 0xb4b9,
                              "LoadGrowthCreatureEvent Query Error : m_id(%d)",
                              sig->m_accId);
        return 0;
    }
    // P222/P214①：ORIG `test %eax; setne; test; je` ⇒ 正极性包裹（!=0 走 for）。
    if (db->get_n_rows() != 0)
    {
        for (unsigned int i = 0; i < db->get_n_rows(); ++i)
        {
            if (!db->fetch())
            {
                return 0;
            }
            if (i > 10)
            {
                return 0;
            }
            // [b28d-solo-DB_Login] P0/P183：ORIG 有具名 char* 局部 -0x10
            // （`i*0x2c; add $0x4150; add sig; add $0x4` ⇒ ptr = 行首）与死 int
            // 局部 -0xc（P232 帧律，`movl $0x0,-0xc(%ebp)`），四个 getter 直接写
            // 字段（ptr / ptr+4 / ptr+8 / ptr+0xc），无 serverId/characNo/jobType
            // 三个临时槽与写回。
            // [solo] P311 数组视图试验：4.4 产 `mov sig; mov i; imul; add $0x4150;
            // add; add $4`（7 条，多一条 mov sig 预载）vs ORIG 6 条
            // （`mov i; imul; add $0x4150; add sig(mem); add $4`）⇒ 劣化，回退。
            // [Lane26 复核] 判据①（jcc/jmp 下标）+ 报告复读：ORIG 0x08448236 链
            // `mov i; imul $0x2c; add $0x4150; add sig; add $4` 的 0x4150/4 是
            // **同一常量 0x4154 的 4.1 拆分**（报告 memset 起点 0x4154 0x1e4 与
            // 四字段目标 0x4154/58/5c/60 实证语义 = 我们的表达式），4.4 恒折
            // `add $0x4154`；视图数组形（/tmp/l26p/p2.cpp g：数组起点 0x4150 +
            // 元素内 +4）虽同构，但要求数组起点 = 0x4150，与 eventCharacInfo 物理
            // 布局（0x4154 起、元素 0x2c、name[30]）不可两全（4+0x2c≠0x2c）⇒ N1 封顶。
            // [DBLogin-V3 第7形·已回滚] 本轮用局部嵌套视图
            // `GCCreatureHolder{pad[0x4150]; GCCreatureList{int count; Elem items[11]}}`
            // 取址实测**可复现 ORIG 的常量拆分** `add $0x4150; …; add $0x4`（证实 ORIG
            // 源为「外层结构体 + 内层元素数组」，且 0x4150/0x4154 是同一元素基址的两段式），
            // 但 4.4 恒先预载 base（`mov 0xc(%ebp),%eax; …; add %edx,%eax`）而 4.1 后置
            // 并以内存操作数 `add 0xc(%ebp),%eax` 收尾 ⇒ 我方 +1 条（146 vs 145，gap -1，
            // identical 不可达）⇒ 回退到 gap 0 形（余 5 行=装载序）。
            // [K2ADBLogin 2026-09-11] 改真实成员数组取址（P235）：ORIG 0x08448236 为
            //   `mov i,%eax; imul $0x2c,%eax,%eax; add $0x4150,%eax; add 0xc(%ebp),%eax;
            //    add $0x4,%eax`（index 先、base 内存操作数后置、常量两段）；
            //   cast 指针形 4.4 恒 base-first + 折叠 `add $0x4154`（旧 5 行失配）。
            char* rowPtr = reinterpret_cast<char*>(&sig->m_growthCreatureEvent.m_entries[i]);
            int rowPad = 0;
            (void)rowPad;
            if (!db->get_short(0, *reinterpret_cast<short*>(rowPtr))) return 0;
            if (!db->get_int(1, *reinterpret_cast<int*>(rowPtr + 4))) return 0;
            if (!db->get_short(2, *reinterpret_cast<int*>(rowPtr + 8))) return 0;
            if (!db->get_str(3, rowPtr + 0xc, 0x1d)) return 0;
        }
    }
    else
    {
        memset(SGBC(sig, OFF_GROWTH_CREATURE), 0, 0x1e4);
    }
    return 1;
}