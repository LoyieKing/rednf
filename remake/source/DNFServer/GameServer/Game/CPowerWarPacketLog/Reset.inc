// ORIG Reset 0x084808de：遍历 map，delete 每个 STPacketLog（析构+释放），
// 最后 map::clear()。
void CPowerWarPacketLog::Reset()
{
    for (std::map<int, STPacketLog*>::iterator it = m_map.begin();
         it != m_map.end(); ++it)
    {
        STPacketLog* log = it->second;
        if (log != 0)
            delete log;
    }
    m_map.clear();
}