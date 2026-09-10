void DB_GuildJoinList::makeRequest(int uid, unsigned int guildKey,
                                   ENUM_SERVER_GROUP group)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x9299), true);

    **guard << 0x21a;                   // op<<(int)
    **guard << uid;                     // op<<(int)
    **guard << guildKey;                // op<<(unsigned int)
    **guard << static_cast<int>(group); // op<<(int)

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
