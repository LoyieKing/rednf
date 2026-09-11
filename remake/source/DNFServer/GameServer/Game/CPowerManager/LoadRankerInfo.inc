void CPowerManager::LoadRankerInfo()
{
    // ORIG 0x0847f4fe：DB 消息 0x165 / -1 / server_group。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DNF_PowerManager.cpp", 0x12e), true);
    **guard << 0x165;
    **guard << -1;
    **guard << G_CEnvironment()->m_serverEnvir.m_serverGroup;   // ORIG env+0x378 直读
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}