int CDungeonEntranceLog::InsertDungeonEntrance()
// [P8S4 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
{
    // [b28d-59] 已还原 int 返回 1（尾 mov %ebx,%eax）、具名 nSize 槽 -0xc、早退
    // `cmpl $0,-0xc; jg body; mov $1,%ebx; jmp end`（125/125 指令数全等）。
    // [L6S3] 残差根因非「编译器临时槽不可控」：ORIG 的 end 是**具名缓存 e**（P13），
    // 故无每轮 end() 的 sret 临时；其槽序 guard -0x14 / it -0x18 / e -0x1c。
    // 探针 /tmp/probe_slot.cpp f1（同工具链 4.4.7 -m32 -O0）实测加具名 e 后
    // 4.4.7 输出同槽（guard -0x14 / it -0x18 / e -0x1c）。
    // 语义：map 非空时把 (key, m_enter, m_partyEnter) 写入 Statistics DB 流
    //（msg 0xe0, LOGDB_Q=4），随后清空 map。
    int nSize = m_inoutMap.size();
    if (nSize <= 0)
        return 1;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x4da), true);
    **guard << 0xe0;
    **guard << -1;
    **guard << nSize;
    // [b28d-59] it/e 提到 for 外（ORIG 槽序 guard -0x14 / it -0x18 / e -0x1c）。
    // [L6S3] e 具名缓存 = P13：ORIG 循环条件为 `it != e` 两槽直比，无每轮 end() 调用。
    std::map<int, STDungeonInoutCnt>::iterator it = m_inoutMap.begin();
    std::map<int, STDungeonInoutCnt>::iterator e = m_inoutMap.end();
    for (; it != e; ++it)
    {
        **guard << it->first;
        **guard << it->second.m_enter;        // 节点 +0x4
        **guard << it->second.m_partyEnter;   // 节点 +0x8
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    m_inoutMap.clear();
    return 1;
}