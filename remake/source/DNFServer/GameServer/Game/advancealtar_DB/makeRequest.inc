// ---- DB_Update（advancealtar__DB_Update 报告）----
// ORIG T 0x0813eb38：opcode 0x1a | uid | memcpy 0x51c → DB_Q。
void DB_Update::makeRequest(int uid, const _CharacAdvanceAltarDbData& data)
{
    // ORIG 无 stream 具名槽：Acquire 结果经 eax 直传 CStreamGuard（P0/P89）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localglobal/global_AdvanceAltar_Dispatcher.cpp", 0x1d9),
                       true);
    guard.operator*()->operator<<(0x1a);
    guard.operator*()->operator<<(uid);
    _CharacAdvanceAltarDbData* sig =
        guard.operator->()->GetInBuffer<_CharacAdvanceAltarDbData>();
    std::memcpy(sig, &data, 0x51c);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}