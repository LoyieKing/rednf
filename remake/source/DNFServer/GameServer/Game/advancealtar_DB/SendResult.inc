bool DB_Load::SendResult(int uid, int uniqueId, _CharacAdvanceAltarDbData& data, bool ok)
{
    if (!ok)          // 残差见上 N122 判词（4.4 bool 取反恒 xor）
        return true;
    // ORIG 无 stream 具名槽：Acquire 结果经 eax 直传（P0/P89）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localglobal/global_AdvanceAltar_Dispatcher.cpp", 0x1bf),
                       true);
    guard.operator*()->operator<<(uid);
    guard.operator*()->operator<<(uniqueId);
    guard.operator->()->put_binary(&data, 0x51c);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);   // ORIG QUEUE_IDX=1
    return true;
