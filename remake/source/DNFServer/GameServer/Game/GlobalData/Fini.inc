// [b28d-solo-GlobalData] 真 bug 修正（SweepGlobalData）：7 个线程对象 ORIG 为
// **delete**（多态 deleting-dtor 虚调用 `mov (%eax),%eax; add $0x8,%eax;
// call *%edx`；ThreadInterface vtable @0x8ce6b90 = [stop@vptr+0, D1@+4,
// D0@+8, pure_virtual@+0xc] 实测），旧重建误写 `->stop()`（vptr+0 虚调，
// 既不析构也不释放）⇒ 每处少 1 条指令，共 gap 7。
void GlobalData::Fini()
{
    delete s_ref_pvp_grade;
    delete s_accept_thr;          // ORIG 0x0829e0e8（vptr+8 虚 D0）
    if (s_monitor_proxy_mgr)
    {
        delete s_monitor_proxy_mgr;
        s_monitor_proxy_mgr = 0;
    }
    if (s_guild_proxy_mgr)
    {
        delete s_guild_proxy_mgr;
        s_guild_proxy_mgr = 0;
    }
    if (s_statistic_proxy_mgr)
    {
        delete s_statistic_proxy_mgr;
        s_statistic_proxy_mgr = 0;
    }
    delete s_udp_thr;             // ORIG 0x0829e196
    delete s_net_thr;             // ORIG 0x0829e1b5
    delete s_dispatch_thr;        // ORIG 0x0829e1d4
    delete s_db_thr_for_logDB;    // ORIG 0x0829e1f3
    delete s_db_thr;              // ORIG 0x0829e212
    delete s_monitor_tcp_thr;     // ORIG 0x0829e231
    delete s_db_mgr;
    delete s_stream_pool;
    delete s_small_stream_pool;
    delete s_big_stream_pool;
    delete s_msgq_mgr;
    delete s_pItemVendingMachine;
    CSyncSlangFilter::FreeInstance();
    delete s_pcryptRijndael_CharacView_;
    delete s_psimpleSSO;
    delete s_pGMAccounts_;
    delete s_GM_Manager;
    delete s_antibotBusinessImpl_;
    delete s_BreakAwaySys;
    delete s_ExpertJobMgr;
    delete s_villageObjectMgr;
    delete s_villageMonsterMgr;
    delete s_revengeDungeonMgr;
    delete s_DailyScheduleManager;
    delete s_onlinePreliminaryTeamMgr;
    delete s_auction_proxy;
    delete s_cera_auction_proxy;
    delete s_db_connector;
    delete s_rentalSystem;
    for (int i = 0; i < 9; ++i)
    {
        if (s_packet_pool_list[i])
        {
            delete s_packet_pool_list[i];
            s_packet_pool_list[i] = 0;
        }
    }
}