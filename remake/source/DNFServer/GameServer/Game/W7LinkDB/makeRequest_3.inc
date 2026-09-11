// ----------------------------------------------------------------------------
// DB_SaveGiveAvengerTitleFlag::makeRequest(int uid, uint accId, uint characNo)
// ORIG @ 0x084422dc（报告 DB_SaveGiveAvengerTitleFlag/makeRequest.md）：
// Acquire("DBThread.cpp", 0xa276) → << 0x28d | << (uint)uid | << accId |
//   << characNo（全部 op<<(unsigned int)，ORIG 080e2292 三连）→ put(DB_Q)。
// 注意：ORIG 此处无 -1 槽。
// ----------------------------------------------------------------------------
void DB_SaveGiveAvengerTitleFlag::makeRequest(int uid, unsigned int accId, unsigned int characNo)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa276), true);

    **guard << 0x28d;                   // op<<(int)
    **guard << uid;                     // op<<(int)（ORIG 080c8c56）
    **guard << accId;                   // op<<(uint)
    **guard << characNo;                // op<<(uint)

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}