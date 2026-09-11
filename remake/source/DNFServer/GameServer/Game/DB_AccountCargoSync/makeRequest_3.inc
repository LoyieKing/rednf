// ============================================================================
// DB_GuildExpBookDelete::makeRequest ORIG @ 0x0843c408
//   opcode 0x209 | -1 | param_1(uint) | param_2(int) | param_3(ENUM_SERVER_GROUP)
//   注意 ORIG 在三个参数前固定写 -1（int）。（分段推断：-1 为 ORIG 常量。）
// ============================================================================
void DB_GuildExpBookDelete::makeRequest(unsigned int param_1, int param_2,
                                        ENUM_SERVER_GROUP param_3)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x902a), true);
    guard.operator*()->operator<<(0x209);                 // SIG_GUILD_EXP_BOOK_DELETE
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(param_1);
    guard.operator*()->operator<<(param_2);
    guard.operator*()->operator<<((int)param_3);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}