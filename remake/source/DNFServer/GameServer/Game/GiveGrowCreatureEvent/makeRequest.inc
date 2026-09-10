void DB_EventSaveGiveGrowCreature::makeRequest(
    unsigned int accId, SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo& info)
{
    // ORIG T 0x08449b32：opcode 0x317 / -1 / accId（无符号 lsEj）/ eventCharacInfo(0x2c)。
    // ORIG 用 operator*()/operator->() 链式（de/pt 调用）；memset 仅清 4 字节。
    // P259：ORIG 无 stream 具名局部（Acquire 结果直传 guard ctor 实参）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb983), true);
    guard.operator*()->operator<<(0x317);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);
    SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo* out =
        guard.operator->()->GetInBuffer<SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo>();
    memset(out, 0, 4);
    memcpy(out, &info, sizeof(info));
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
