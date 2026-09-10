int Timer_DungeonInoutCloseTime::dispatch_sig(int p1, int p2, unsigned int p3)
{
    (void)p1; (void)p2;
    int dungeonIdx;   // [C05② 命中] 原 unsigned int（ORIG -0x58：声明序最先）
    time_t now = static_cast<time_t>(GlobalData::s_systemTime_.getCurSec());
    struct tm tmBuf;
    struct tm* ptm = localtime_r(&now, &tmBuf);
    const int curHour = ptm->tm_hour;
    const int curMin = ptm->tm_min;
    dungeonIdx = p3;   // ORIG -0x58 具名副本（P0 槽往返）
    if (dungeonIdx == 0)
        return 0;

    std::map<int, STDungeonInoutTime>::iterator it =
        G_CDataManager()->m_serverParameter.m_dungeonInoutc8.find(dungeonIdx);
    if (it != G_CDataManager()->m_serverParameter.m_dungeonInoutc8.end())
    {
        ptm->tm_hour = curHour + 1;
        // ORIG 0863a29c `mov 0xc(%eax),%edx`（Ghidra C：*(int*)(it->+0xc)）⇒
        // pair+0xc = second+8 = m_startMin（旧码 m_endMin 产 +0x10，与 ORIG 不符）。
        ptm->tm_min = it->second.m_startMin;
        ptm->tm_sec = 0;
        const time_t closeSec = mktime(ptm);
        const int delay = static_cast<int>(closeSec - now);

        if (delay < 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x14a3, 0);
            trace("ULTIMATE_LOG : TIMER OFF!!");
            return 0;
        }
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x93,
                                    delay, 0, dungeonIdx);
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x14b1, 0);
            trace("ULTIMATE_LOG : ULTIMATE DUNGEON OPEN TIMER ON!!");
        }

        // !(IsPowerWarEventOn() || s_villageMonsterMgr->OnEvent())
        if (!GlobalData::s_power_manager->IsPowerWarEventOn() &&
            !GlobalData::s_villageMonsterMgr->OnEvent())
        {
            G_CDataManager()->m_serverParameter.setDungeonOpen(dungeonIdx, false);
            G_GameWorld()->send_all_dungeon_inout_message(dungeonIdx);
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x14b8, 0);
                trace("ULTIMATE_LOG : ULTIMATE DUNGEON CLOSE!!");
            }
        }
    }
    return 1;
