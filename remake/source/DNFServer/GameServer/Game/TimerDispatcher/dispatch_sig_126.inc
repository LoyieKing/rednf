int Timer_DungeonInoutOpenTime::dispatch_sig(int p1, int p2, unsigned int p3)
{
    (void)p1; (void)p2;
    // ORIG 08639e20：getCurSec 结果单槽 -0x78（int/time_t 同宽），localtime_r 直取
    // 该槽地址；旧码 curT+nowSec 双具名多一槽两条 mov。
    const int curT = static_cast<int>(GlobalData::s_systemTime_.getCurSec());  // ORIG -0x78(%ebp)
    struct tm timeBuf;                                 // ORIG -0xac(%ebp)
    struct tm* localTm = localtime_r(reinterpret_cast<const time_t*>(&curT), &timeBuf); // ORIG ptr → -0x20(%ebp)
    const int hour = localTm->tm_hour;                 // ORIG -0x18(%ebp)（tm+0x8）
    const int min = localTm->tm_min;                   // ORIG -0x14(%ebp)（tm+0x4）
    // P0：ORIG 08639e4d `mov 0x14(%ebp),%eax; mov %eax,-0x7c(%ebp)` 且后续 6 处
    // 均读 -0x7c（find key 亦 lea -0x7c）⇒ 原码有 p3 的具名副本。
    const int dungeonIdx = static_cast<int>(p3);       // ORIG -0x7c(%ebp)
    if (dungeonIdx == 0)
    {
        return 0;
    }

    // ORIG：find/end 各自写完整表达式（两次 G_CDataManagerv 调用，不缓存）
    std::map<int, STDungeonInoutTime>::iterator it =
        G_CDataManager()->m_serverParameter.m_dungeonInoutc8.find(dungeonIdx);
    if (it != G_CDataManager()->m_serverParameter.m_dungeonInoutc8.end())
    {
        // second 自节点数据 +0x04 起：endHour@+8 / startHour@+4 /
        // startMin@+c / endMin@+10（对照 asm 取值偏移）
        if (it->second.m_endHour < hour)
        {
            localTm->tm_hour = it->second.m_startHour;   // ORIG 写 tm+0x8
            localTm->tm_min = it->second.m_startMin;     // ORIG 写 tm+0x4
            localTm->tm_sec = 0;
            // [fix] ORIG 08639f1a `addl $0x15180,-0x1c(%ebp)`（内存自增，紧跟
            // `mov %eax,-0x1c(%ebp)` 的 mktime 返回值 store）⇒ 原码两句式；
            // 旧码单表达式生 `add $0x15180,%eax`（ORIG-only addl / OURS-only add 各 1）。
            int nextOpenT = mktime(localTm);
            nextOpenT += 0x15180;   // +86400 秒（次日同刻）
            int delta = nextOpenT - curT;                // ORIG -0xc(%ebp)
            if (delta < 0)
            {
                cMyTrace trace(__PRETTY_FUNCTION__,
                               0x1445, 0);
                trace("ULTIMATE_LOG : TIMER OFF!!");
                return 0;
            }
            G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                        (TIMER_MESSAGE)0x93, delta, 0, dungeonIdx);
            cMyTrace trace(__PRETTY_FUNCTION__,
                           0x1454, 0);
            trace("ULTIMATE_LOG : NEXT DAY TIMER ON!!");
            return 1;
        }
        localTm->tm_hour = hour;                          // ORIG 写 tm+0x8
        localTm->tm_min = it->second.m_endMin;            // ORIG 写 tm+0x4（v+0x10）
        localTm->tm_sec = 0x3b;
        int checkT = mktime(localTm);                     // ORIG -0x1c(%ebp)
        int delta = checkT - curT;                        // ORIG -0x10(%ebp)
        if (delta < 0)
        {
            delta = 0;
        }
        // [S1 实验] ORIG 0863a0a6 前段 `mov -0x78(%ebp),%ebx`（curT 跨 cMyTrace ctor
        // 保活，实参位 0x10(%esp) 读 ebx）⇒ P227 全表达式临时链形；旧码具名两语句。
        cMyTrace(__PRETTY_FUNCTION__, 0x1462, 0)(
            "DELTA %d , CHECK_T %d , CUR_T %d\\n", delta, checkT, curT);
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x94, delta, 0, dungeonIdx);
        cMyTrace traceClose(__PRETTY_FUNCTION__,
                            0x146e, 0);
        traceClose("ULTIMATE_LOG : ULTIMATE DUNGEON CLOSE TIMER ON!!");
        if (!GlobalData::s_power_manager->IsPowerWarEventOn() &&
            !GlobalData::s_villageMonsterMgr->OnEvent())
        {
            // ORIG 082687fc：dm+0x68（m_serverParameter）isDungeonOpen(0x2aff)；
            // ORIG 0826868c（0863a15c..0863a176）：setDungeonOpen(**dungeonIdx**,true)
            // —— `mov -0x7c(%ebp),%ebx; call G_CDataManager; add $0x68,%eax;
            // movl $0x1,0x8(%esp); mov %ebx,0x4(%esp); mov %eax,(%esp)` 实证
            // 第1实参 = -0x7c 具名副本（Ghidra C 同为 local_80），旧码 0x2aff 是
            // 与 isDungeonOpen 混淆的错常量（真 bug：开启的是错误 dungeon 开关）。
            if (!G_CDataManager()->m_serverParameter.isDungeonOpen(0x2aff))
            {
                G_CDataManager()->m_serverParameter.setDungeonOpen(dungeonIdx, true);
                G_GameWorld()->send_all_dungeon_inout_message(dungeonIdx);
            }
            cMyTrace traceOpen(__PRETTY_FUNCTION__,
                               0x1478, 0);
            traceOpen("ULTIMATE_LOG : ULTIMATE DUNGEON OPEN!!");
        }
    }
    return 1;
