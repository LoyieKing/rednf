// DspD4 B4：DB_JoinGuildInfo / DB_CancelJoinGuild makeRequest 定义
// （ORIG _ZN16DB_JoinGuildInfo11makeRequestEi17ENUM_SERVER_GROUPj @0x0843d5fa、
//  _ZN18DB_CancelJoinGuild11makeRequestEijj @0x0843d78c；nm 核实。包构建范式
//  同 DB_AccountCargoSync.cpp：StreamPool::Acquire → CStreamGuard → DB_Q 入队。）
void DB_JoinGuildInfo::makeRequest(int uid, ENUM_SERVER_GROUP serverGroup, unsigned int characNo)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x9218), true);
    **guard << 0x219;               // opcode（ORIG lsEi）
    **guard << uid;                 // int
    **guard << serverGroup;         // ENUM_SERVER_GROUP（int 重载）
    **guard << characNo;            // unsigned 重载（ORIG lsEj）
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}