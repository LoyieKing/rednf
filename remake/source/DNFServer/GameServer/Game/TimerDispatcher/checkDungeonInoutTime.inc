bool Timer_DungeonInoutOpenTime::checkDungeonInoutTime(int dungeonIdx)
{
    const time_t nowSec = static_cast<time_t>(GlobalData::s_systemTime_.getCurSec());
    struct tm timeBuf;
    struct tm* localTm = localtime_r(&nowSec, &timeBuf);
    const int hour = localTm->tm_hour;
    const int min = localTm->tm_min;
    std::map<int, STDungeonInoutTime>::iterator it =
        G_CDataManager()->m_serverParameter.m_dungeonInoutc8.find(dungeonIdx);
    if (it != G_CDataManager()->m_serverParameter.m_dungeonInoutc8.end())
    {
        // [b28d-solo-TimerDispatcher2] P171 死写入照抄：ORIG 08639ab0 `mov (%eax),%eax;
        // mov %eax,-0xc(%ebp)` 读 pair 首字段（key）后不再使用（第 5 次 ptEv 来源）。
        const int itKey = it->first;
        (void)itKey;
        // [b28d-solo-TimerDispatcher] P56：ORIG 尾段 `mov $0; test %al; je; mov $1;
        // jmp; mov $0` = `if (…) return true; return false;`（直 return 表达式
        // 4.4 恒折直跳，无 bool 归一岛）。
        if (hour >= it->second.m_startHour && hour <= it->second.m_endHour
            && min >= it->second.m_startMin && min < it->second.m_endMin)
            return true;
    }
    return false;
