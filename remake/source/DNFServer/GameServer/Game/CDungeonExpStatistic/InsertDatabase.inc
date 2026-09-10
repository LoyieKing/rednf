void CDungeonExpStatistic::InsertDatabase()
{
    // [B24D判词 N] 语义正确：m_map 判空早返+逐记录构包+DB insert 全链；
    //   P167 调用点差集 31/31 CLEAR；diff=地址/EH lowering。
    if (m_map.size() == 0)
        return;
    // e1/e2/e3 按 ORIG 栈槽分配序（gcc4.1 由低到高 e3/e2/e1）倒序声明，
    // gcc4.4 按声明序从高地址往下分配，倒排后槽位与 ORIG 对齐。
    stDungeonExp e3;
    stDungeonExp e2;
    stDungeonExp e1;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x59c),
                       true);
    **guard << 0x116;
    **guard << -1;
    **guard << (int)m_map.size();
    // b28d-61 [P212/N98：4.4 槽按声明序先声明深]：ORIG it@-0x38（begin 先调用）、
    // endIt@-0x3c；旧形 endIt 先声明致两槽互换（NEAR 9）。it 提前声明（仍直构，
    // 非先声明后赋值，不触发 P50 默认 ctor 弱实例）后槽位与 ORIG 一致。
    std::map<int, stDungeonExp>::iterator it = m_map.begin();
    std::map<int, stDungeonExp>::iterator endIt = m_map.end();
    for (; it != endIt; ++it)
    {
        **guard << it->first;
        e1 = it->second;
        **guard << e1.m_exp;
        e2 = it->second;
        **guard << e2.m_count;
        e3 = it->second;
        **guard << e3.m_clearTimes;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    m_map.clear();
