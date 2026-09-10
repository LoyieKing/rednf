void DB_EventUpdateGiveGrowCreature::makeRequest(
    unsigned int accId, SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo& info)
{
    // ORIG T 0x08449d1c：与 save 同构（同一 opcode 0x317 组）。
    // P259：同 save（无 stream 具名局部）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb9aa), true);
    guard.operator*()->operator<<(0x317);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);
    SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo* out =
        guard.operator->()->GetInBuffer<SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo>();
    memset(out, 0, 4);          // ORIG 0x08449d?? `movl $0x4,0x8(%esp)`（照抄，非 sizeof）
    memcpy(out, &info, sizeof(info));
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
