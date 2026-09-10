void CLevelDungeonPlayStatistic::InsertDatabase()
{
    if (m_levelPlayMap.size() == 0)
        return;
    // ORIG 栈槽（低→高）：end@-0x18、it@-0x14、guard@-0x10；
    // gcc4.4 按声明序从高地址往下分配，故声明序 guard → it → endIt。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x569),
                       true);
    **guard << 0x107;
    **guard << -1;
    **guard << (int)m_levelPlayMap.size();
    std::map<int, stDungeonPlayInfo>::iterator it = m_levelPlayMap.begin();
    std::map<int, stDungeonPlayInfo>::iterator endIt = m_levelPlayMap.end();
    while (it != endIt)
    {
        // pair 起始即 key；value（stDungeonPlayInfo）@+4，故 value+4=m_userCount、
        // value+8=m_playCount（ORIG 经 iterator::operator-> 读 +0/+4/+8）。
        **guard << it->first;
        **guard << it->second.m_userCount;
        **guard << it->second.m_playCount;
        ++it;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    m_levelPlayMap.clear();
