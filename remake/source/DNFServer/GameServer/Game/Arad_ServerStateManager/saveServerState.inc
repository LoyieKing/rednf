// ---------------------------------------------------------------------------
// saveServerState —— ORIG 0x081a9afa：
// 渠道号==10 时：SigServerState::set(endFlag, category, code, msg) →
// Acquire(line 0xb2) → make_internal_stream_jpn(guard, K_SAVE_SERVER_STATE(0x2a),
// -1) → put_binary(sig, 0x195) → MsgQueueMgr::put(DB_Q)。
// ---------------------------------------------------------------------------
void Arad_ServerStateManager::saveServerState(ENUM_NOTIFY_EVENT_CATEGORY category,
                                              ENUM_GLOBAL_EFFECT_ARAD code,
                                              const AradServerStateMessage& msg,
                                              bool endFlag)
{
    if (G_CEnvironment()->get_channel_no() != 10)
        return;

    arad::SigServerState sig;
    sig.set(endFlag, (int)category, (int)code, msg);
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localjapan/Arad_ServerStateManager.cpp", 0xb2),
                       true);
    DISPATCHER::make_internal_stream_jpn(guard, K_SAVE_SERVER_STATE, -1);
    guard.operator->()->put_binary(&sig, 0x195);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}