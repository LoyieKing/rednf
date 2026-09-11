int CDungeonEntranceLog::InsertDungeonEntrance_Hour()
// [P8S4 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
{
    // [b28d-59] 与 InsertDungeonEntrance 同构——已还原 int 返回 1、具名 nSize 槽 -0xc、
    // 早退 `jg body; mov $1,%ebx`。[L6S3] 残差根因同件 = ORIG 的 end 为具名缓存 e
    //（P13），非编译器临时槽（探针 /tmp/probe_slot.cpp f1/f2 实测 4.4.7 同槽）。
    // 语义：map 非空时把 (key, m_total, m_curInside) 写入 Statistics DB 流
    //（msg 0xe1, LOGDB_Q=4），随后归零 m_total（ORIG 清节点 +0xc）。
    int nSize = m_inoutMap.size();
    if (nSize <= 0)
        return 1;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x4b5), true);
    **guard << 0xe1;
    **guard << -1;
    **guard << nSize;
    // [b28d-59] it/e 提到 for 外（同 InsertDungeonEntrance 的 ORIG 槽序）。
    std::map<int, STDungeonInoutCnt>::iterator it = m_inoutMap.begin();
    std::map<int, STDungeonInoutCnt>::iterator e = m_inoutMap.end();
    for (; it != e; ++it)
    {
        **guard << it->first;
        **guard << it->second.m_total;       // 节点 +0xc
        **guard << it->second.m_curInside;   // 节点 +0x10
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    // [L6S3] 第二循环复用同一 it/e：ORIG 0860e516 `lea -0x2c` 临时 + `mov -0x2c→-0x18`
    // 拷贝 = 赋值形 `it = m_inoutMap.begin()`（N86，4.1 不做 copy-elision）；
    // 探针 /tmp/probe_slot.cpp f2 逐条复现该槽与形态。
    for (it = m_inoutMap.begin(); it != e; ++it)
        it->second.m_total = 0;
    return 1;
}