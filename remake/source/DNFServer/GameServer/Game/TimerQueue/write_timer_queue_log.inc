void TimerQueue::write_timer_queue_log() throw()
{
    // b28d-solo-TimerQueue 判词：AE（130=130，P227 全表达式临时链）。原形「具名
    // const char* str + 具名 cMyTrace trace」使 4.4 把 c_str 结果落槽（-0x10/-0xc）
    // 并把 trace 临时压深（-0x44/-0x54）——即 b28d-49「N82 整体浅移 0x18」判词的根因；
    // 改 cMyTrace(...)(...) 后 4.4 在 ctor 前求值 c_str 并以 %ebx 跨 ctor 保活、
    // trace 临时落 -0x2c/-0x18，与 ORIG 逐条 AE。@0x863101e
    std::string log;
    log.clear();

    std::map<int, int>::iterator it = m_map.begin();
    std::map<int, int>::iterator end = m_map.end();
    for (; it != end; it++)
    {
        char buf[20] = { '\0' };
        sprintf(buf, "(%d %d) ", it->first, it->second);
        log += buf;

        // P23：ORIG 仅当 log 长度 > 0x800 才构造 cMyTrace 并输出。
        if (log.length() > 0x800)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x101, 0)("TIMER_QUEUE_LOG : %s", log.c_str());
        }
        log.clear();
    }

    if (log.length() != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x107, 0)("TIMER_QUEUE_LOG : %s", log.c_str());
    }
}