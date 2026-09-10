int CDungeonEntranceLog::InsertDungeonEntrance_Hour()
{
    // [b28d-59] 封 N82附/N98（ORIG 0x860e38e）：与 InsertDungeonEntrance 同构——已还原
    // int 返回 1、具名 nSize 槽 -0xc、早退 `jg body; mov $1,%ebx`；残差同件（4.4 编译器
    // 临时 end() sret 占浅槽致 guard/it 下沉），语义等价。语义：map 非空时把
    // (key, m_total, m_curInside) 写入 Statistics DB 流（msg 0xe1, LOGDB_Q=4），
    // 随后归零 m_total（ORIG 清节点 +0xc）。
    int nSize = m_inoutMap.size();
    if (nSize <= 0)
        return 1;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x4b5), true);
    **guard << 0xe1;
    **guard << -1;
    **guard << nSize;
    // [b28d-59] it 提到 for 外（同 InsertDungeonEntrance 的 ORIG 槽序）。
    std::map<int, STDungeonInoutCnt>::iterator it = m_inoutMap.begin();
    for (; it != m_inoutMap.end(); ++it)
    {
        **guard << it->first;
        **guard << it->second.m_total;       // 节点 +0xc
        **guard << it->second.m_curInside;   // 节点 +0x10
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    for (std::map<int, STDungeonInoutCnt>::iterator it = m_inoutMap.begin();
         it != m_inoutMap.end(); ++it)
        it->second.m_total = 0;
    return 1;
