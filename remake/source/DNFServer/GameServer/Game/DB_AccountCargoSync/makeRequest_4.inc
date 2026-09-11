// ============================================================================
// DB_InsertRandomOptionItemInform::makeRequest ORIG @ 0x08442d70
//   opcode 0x299 | param_1(int uid) | param_2(uint accId) | param_3(uchar 零扩展 uint)
// ============================================================================
void DB_InsertRandomOptionItemInform::makeRequest(int param_1,
                                                  unsigned int param_2,
                                                  unsigned char param_3)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa488), true);
    guard.operator*()->operator<<(0x299);                 // SIG_INSERT_RANDOM_OPTION_ITEM_INFORM
    guard.operator*()->operator<<(param_1);
    guard.operator*()->operator<<(param_2);
    guard.operator*()->operator<<(param_3);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}