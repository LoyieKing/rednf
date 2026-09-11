// ----------------------------------------------------------------------------
// DB_UpdateBlueMarbleEnterCount::makeRequest(unsigned int accId,
//                                            unsigned char count)
// ORIG T 0x0844884c `_ZN29DB_UpdateBlueMarbleEnterCount11makeRequestEjh`
// （objdump 逐指令转写）：Acquire("DBThread.cpp", 0xb585) → guard(stream,true)
//   → << (int)0x30c | << (int)-1 | << (uint)accId | << (uchar)count
// → MsgQueueMgr::put(QUEUE_IDX(2)=DB_Q, guard)。
// ----------------------------------------------------------------------------
void DB_UpdateBlueMarbleEnterCount::makeRequest(unsigned int accId,
                                                unsigned char count)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb585), true);
    guard.operator*()->operator<<(0x30c);   // SIG opcode
    guard.operator*()->operator<<(-1);      // 固定 -1
    guard.operator*()->operator<<(accId);   // 参数1（unsigned int）
    guard.operator*()->operator<<(count);   // 参数2（unsigned char）

    GlobalData::s_msgq_mgr->put(
        static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
}