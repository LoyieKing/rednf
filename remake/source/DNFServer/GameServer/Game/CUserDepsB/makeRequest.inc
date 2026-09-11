// ORIG T 0x0844cc2e sz 0x10a："DBThread.cpp"/0xc25a 流，头 0x34f + 四 int，
// 入 DB_Q（QUEUE_IDX 2）。流序 = (accId, characNo, level, state)：
// W8LT_DBTask.cpp DB_SaveEventLevelupSupport::dispatch 读流 characNo→level→state
// （ORIG 0x0844cafe）实证第 2/3 参语义，形参名据此修正。
void DB_SaveEventLevelupSupport::makeRequest(int accId, int characNo,
                                             int level, int state)
{
    // b28d-solo-CUserDepsB: AE（73=73，本轮由 NEAR 翻正）——ORIG 0x0844cc90/0x0844ccaa
    // 第 3/4 次 `operator<<` 的实参槽是 0xc(%ebp)(characNo) 与 0x10(%ebp)(level)：
    // 原代码按注释写的流序 `<< level; << characNo;` 与形参序相反（写流序实为
    // accId→characNo→level→state），交换两句即与 ORIG 逐条全等。p167 CLEAR（16/16）。
    // ORIG 0x0844cc2e。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xc25a), true);
    **guard << 0x34f;
    **guard << accId;
    **guard << characNo;
    **guard << level;
    **guard << state;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}