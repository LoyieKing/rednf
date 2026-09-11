// ============================================================================
// DB_UpdateRevengeData::makeRequest —— ORIG 0x0843f232（报告 makeRequest.md 全文）
// ============================================================================
// ORIG 0x843f232：Acquire("DBThread.cpp",0x98bd) → CStreamGuard(stream,true)
//   → <<0x244(int) → <<-1(int) → <<characNo(uint) → <<attack(uchar 零扩)
//   → <<revenge(uchar 零扩) → put(DB_Q(2))。
// ============================================================================
void DB_UpdateRevengeData::makeRequest(unsigned int characNo,
                                       unsigned char attack,
                                       unsigned char revenge)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x98bd), true);
    guard.operator*()->operator<<((int)0x244);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<((unsigned int)characNo);
    guard.operator*()->operator<<(attack);
    guard.operator*()->operator<<(revenge);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}