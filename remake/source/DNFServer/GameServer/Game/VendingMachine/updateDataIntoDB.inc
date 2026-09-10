void CVendingMachineStatistic::updateDataIntoDB()
{
    // ORIG：Acquire Stream("Statistics.cpp",0x624) → CStreamGuard(stream,true) →
    // 写 SIG=0x163, -1, m_useCount, m_output.size(), 每键值对 →
    // MsgQueueMgr::put(LOGDB_Q) → _init()。
    Stream* stream = GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x624);
    CStreamGuard guard(stream, true);

    **guard << 0x163;                    // SIG（DB 侧据此路由）
    **guard << (int)-1;                  // 固定 -1
    **guard << m_useCount;               // 使用次数
    **guard << (unsigned int)m_output.size();

    for (std::map<unsigned long, unsigned int>::const_iterator it =
             m_output.begin();
         it != m_output.end(); ++it)
    {
        **guard << it->first;            // 物品索引（unsigned long）
        **guard << it->second;           // 数量（unsigned int）
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    _init();
