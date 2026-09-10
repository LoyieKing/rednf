int CDungeonEntranceLog::InsertDungeonEntrance()
{
    // [b28d-59] 封 N82附/N98（ORIG 0x860e59c）：已还原 int 返回 1（尾 mov %ebx,%eax）、
    // 具名 nSize 槽 -0xc、早退 `cmpl $0,-0xc; jg body; mov $1,%ebx; jmp end`（125/125
    // 指令数全等）。残差=槽位相对序：4.4 把编译器临时 end() sret 放浅槽 -0x10、具名
    // guard/it 下沉（-0x18/-0x1c），4.1 为 guard -0x14 / it -0x18 / end 临时 -0x1c
    // （声明序）；源码不可控编译器临时槽（P232/N98），语义等价。
    // 语义：map 非空时把 (key, m_enter, m_partyEnter) 写入 Statistics DB 流
    //（msg 0xe0, LOGDB_Q=4），随后清空 map。
    int nSize = m_inoutMap.size();
    if (nSize <= 0)
        return 1;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x4da), true);
    **guard << 0xe0;
    **guard << -1;
    **guard << nSize;
    // [b28d-59] it 提到 for 外（ORIG 槽序 guard -0x14 / it -0x18 / end 临时 -0x1c）。
    std::map<int, STDungeonInoutCnt>::iterator it = m_inoutMap.begin();
    for (; it != m_inoutMap.end(); ++it)
    {
        **guard << it->first;
        **guard << it->second.m_enter;        // 节点 +0x4
        **guard << it->second.m_partyEnter;   // 节点 +0x8
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    m_inoutMap.clear();
    return 1;
