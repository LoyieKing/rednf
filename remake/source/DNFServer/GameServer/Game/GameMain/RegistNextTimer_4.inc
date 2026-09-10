void Timer_DungeonInoutOpenTime::RegistNextTimer()
{
    std::time_t now = static_cast<std::time_t>(s_systemTime_.getCurSec());
    std::tm t;
    std::tm* p = localtime_r(&now, &t);   // P43：ORIG 经返回值指针读字段
    const int curHour = p->tm_hour;
    const int curMin = p->tm_min;
    // P183：ORIG 无 table 引用缓存——begin/end 各自独立调 G_CDataManager()
    // （p167 差集 MISSING 1×G_CDataManager 即此）。
    std::map<int, STDungeonInoutTime>::iterator it =
        G_CDataManager()->m_serverParameter.m_dungeonInoutc8.begin();
    std::map<int, STDungeonInoutTime>::iterator e =
        G_CDataManager()->m_serverParameter.m_dungeonInoutc8.end();
    for (; it != e; ++it)
    {
        int key = it->first;   // ORIG -0x10 槽（循环头一次性读取）
        std::time_t next;
        // P157/P183：ORIG 每个字段访问都重新 `call operator->`（14×，p167 差集
        // MISSING 14×ptEv）——禁 `const T& win = it->second` 引用缓存。
        if (curHour >= it->second.m_startHour &&
            curHour <= it->second.m_endHour &&
            curMin >= it->second.m_startMin &&
            curMin <= it->second.m_endMin)
        {
            p->tm_hour = 0;
            p->tm_min = 0;
            p->tm_sec = 0;
            next = mktime(p);
        }
        else
        {
            const bool beforeStart =
                it->second.m_startHour >= curHour ||
                (it->second.m_endHour >= curHour &&
                 (it->second.m_startMin > curMin ||
                  (it->second.m_startMin <= curMin && it->second.m_endMin < curMin)));
            if (beforeStart)
            {
                // ORIG 0x08639cc5/0x08639cf3：`cmp curHour; setg %al; test; je`
                // —— 条件写作 startHour > curHour（P225 物化方向）。
                if (it->second.m_startHour > curHour)
                    p->tm_hour = it->second.m_startHour;
                else if (it->second.m_startMin > curMin)
                    p->tm_hour = curHour;
                else
                    p->tm_hour = curHour + 1;
                p->tm_min = it->second.m_startMin;
                p->tm_sec = 0;
                next = mktime(p);
            }
            else
            {
                p->tm_hour = it->second.m_startHour;
                p->tm_min = it->second.m_startMin;
                p->tm_sec = 0;
                next = mktime(p) + 0x15180;  // +86400：明天同一开放点
            }
        }
        int delay = static_cast<int>(next) - static_cast<int>(now);
        if (delay < 0)
        {
            delay = 0;
        }
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                    (TIMER_MESSAGE)0x93, delay, 0,
                                    (unsigned int)key);
    }
