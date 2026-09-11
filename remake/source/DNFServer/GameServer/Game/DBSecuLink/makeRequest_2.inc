// ----------------------------------------------------------------------------
// DB_GuildJoinList::makeRequest(int uid, uint guildKey, ENUM_SERVER_GROUP group)
// ORIG @ 0x0843dd7c（报告 DB_GuildJoinList/makeRequest.md）：
// Acquire("DBThread.cpp", 0x9299) → << 0x21a | << uid(int) | << guildKey(uint) |
// << group（op<<(int)，参数槽按 int）→ put(DB_Q)。无 GetInBuffer。
// ----------------------------------------------------------------------------
void DB_GuildJoinList::makeRequest(int uid, unsigned int guildKey,
                                   ENUM_SERVER_GROUP group)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x9299), true);

    **guard << 0x21a;                   // op<<(int)
    **guard << uid;                     // op<<(int)
    **guard << guildKey;                // op<<(unsigned int)
    **guard << static_cast<int>(group); // op<<(int)

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}