void CPowerManager::LoadPowerWarInfo()
{
    // ORIG 0x08480d98：DB 消息 SIG_LOAD_POWER_WAR 0x19b / -1 / server_group。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DNF_PowerManager.cpp", 0x3c4), true);
    **guard << 0x19b;
    **guard << -1;
    **guard << G_CEnvironment()->m_serverEnvir.m_serverGroup;   // ORIG env+0x378 直读
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
