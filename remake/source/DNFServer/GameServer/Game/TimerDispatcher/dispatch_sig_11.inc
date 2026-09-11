// ORIG T 0x08633750 (_ZN17TimerFatigueReset12dispatch_sigEiij)
// [b28d-30] 复核 DIFF：callee-saved 寄存器缓存/帧差（N7/N82/P231 族），ORIG 0x08633750。
// [b28d-solo-TimerDispatcher] 判词 N7/N82 封顶：callee-saved 缓存组 + 帧差（167/169）。
bool TimerFatigueReset::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // ORIG：G_CEnvironment()+0x37c（每日重置小时）即 m_serverEnvir.m_fatigueTime；
    // ORIG 每处均重新取 env。
    // ORIG 0x0863375d `mov 0x37c(%eax),%ebx` 单段直落 callee-saved 跨 update 调用，
    // 无栈槽 ⇒ P0 表达式内联（旧具名局部多 -0x10 槽并使 guard 槽 -0x14→-0x18 全帧平移）。
    static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())
        ->update_time_values(G_CEnvironment()->m_serverEnvir.m_fatigueTime);

    // ORIG 每个 GameWorld 成员调用前都重新取 G_GameWorld()，逐条保留。
    G_GameWorld()->ResetAllFatigue();
    G_GameWorld()->ResetAllCoins();
    G_GameWorld()->DailyEventModify();
    G_GameWorld()->ResetAllEventData();
    G_GameWorld()->ReselectDailyQuest();
    G_GameWorld()->DeleteDailyItem();
    G_GameWorld()->ResetAllCleadpadPoint();
    G_GameWorld()->ResetAllPassPadFailCount();
    G_GameWorld()->AddDailyItem();
    G_GameWorld()->resetDailyData();
    G_GameWorld()->ResetEntireReliablePerson();   // ORIG 0x8633806：resetDailyData 之后立即调用
    GetInstanceDungeonEntranceLog()->InsertDungeonEntrance();
    GetInstanceCommonStatisticsMgr()->InsertDatabase();

    // ORIG：三个统计域上报（CLevelDungeonPlayStatistic@+0x0、CDungeonExpStatistic@+0x18、
    // CExpertJobStatistic 单例），与上面两个 InsertDatabase 同批落库。
    ((CLevelDungeonPlayStatistic*)GlobalData::s_statisticMgr)->InsertDatabase();
    ((CDungeonExpStatistic*)((char*)GlobalData::s_statisticMgr + 0x18))->InsertDatabase();
    GetInstanceExpertJobStatistic()->UpdateExpertJob();

    advancealtar::HistoryLog::updateStatistics();   // 已声明，定义未链接 → UNLINKED_CALLEE

    // ORIG 0x0863386c-0x086338fd：Acquire 结果 %eax 直落 guard ctor 0x4(%esp)
    // （技巧36）；guard1 槽 -0x14、guard2 槽 -0x1c 紧邻、dtor+catch 各随块尾
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp", 0x512), true);
        // ORIG 0x08633888-0x086338b9：每写经显式 operator*() 调用（跨版本代理链
        // 形，同 CUserOnlyOrig LoadMileage 模板；guard<<x 直调会丢 op* 段）
        guard.operator*()->operator<<((int)0x4e);
        guard.operator*()->operator<<((int)-1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q /*2*/, guard);
    }

    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp", 0x51c), true);
        guard.operator*()->operator<<((int)0x50);
        guard.operator*()->operator<<((int)-1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q /*4*/, guard);
    }

    DB_LoadAutoMarketConditionsControl::makeRequest(false);   // 定义 W9_EventSmall.cpp:30
    DB_CreateCharac::makeRequestResetLimitCreateMid();        // 定义 DB_CreateCharac.cpp
    G_GameWorld()->ResetOneADayItemShop();

    // ORIG 0x086339d9-0x086339e7：bool 返回 byte 槽 -0x9 物化 + `movzbl; xor $0x1;
    // test; je TRUE岛(mov$1)`——false fall-through（技巧1/22 同族）
    bool ok = RegistNextTimer();
    if (ok != 1)
        return 0;
    return 1;
}