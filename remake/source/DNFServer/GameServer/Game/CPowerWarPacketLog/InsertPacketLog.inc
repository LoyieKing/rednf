// ORIG InsertPacketLog 0x08480970：以 (unsigned int)t 为 key find；
// 命中 → ++(元素->m_count)；未命中 → new STPacketLog(0x14) 初始化
// （m_count=1、m_field8=t、m_sendType=type、m_field4=i、m_name.assign(name)）
// 后 make_pair(t, log) insert。
void CPowerWarPacketLog::InsertPacketLog(int i, unsigned short t,
                                         eSendType type, char const* name)
{
    std::map<int, STPacketLog*>::iterator it = m_map.find(t);
    if (it != m_map.end())
    {
        STPacketLog* found = it->second;
        found->m_count = found->m_count + 1;
    }
    else
    {
        STPacketLog* log = new STPacketLog;
        log->m_count = 1;
        log->m_field8 = t;
        log->m_sendType = type;
        log->m_field4 = i;
        log->m_name.assign(name);
        m_map.insert(std::make_pair(t, log));
    }
}