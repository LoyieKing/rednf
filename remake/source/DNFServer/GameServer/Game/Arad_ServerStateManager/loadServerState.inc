void Arad_ServerStateManager::loadServerState()
{
    arad::SigServerState sig;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localjapan/Arad_ServerStateManager.cpp", 0x6a),
                       true);
    DISPATCHER::make_internal_stream_jpn(guard, K_LOAD_SERVER_STATE, -1);
    guard.operator->()->put_binary(&sig, 0x195);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
