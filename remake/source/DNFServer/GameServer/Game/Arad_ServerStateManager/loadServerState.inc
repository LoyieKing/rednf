// ---------------------------------------------------------------------------
// loadServerState —— ORIG 0x081a97f8：
// SigServerState 初值（流内 GetInBuffer 反序列化用），从 StreamPool 取流 →
// make_internal_stream_jpn(guard, K_LOAD_SERVER_STATE(0x29), -1) →
// put_binary(sig, 0x195) → MsgQueueMgr::put(DB_Q(2))。
// ORIG 走 guard.operator->() 链（0x80c8c4e）再 put_binary（P103）。
// ---------------------------------------------------------------------------
void Arad_ServerStateManager::loadServerState()
{
    arad::SigServerState sig;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localjapan/Arad_ServerStateManager.cpp", 0x6a),
                       true);
    DISPATCHER::make_internal_stream_jpn(guard, K_LOAD_SERVER_STATE, -1);
    guard.operator->()->put_binary(&sig, 0x195);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}