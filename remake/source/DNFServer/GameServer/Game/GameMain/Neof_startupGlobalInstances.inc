// ---------------------------------------------------------------------------
// Neof_startupGlobalInstances() —— ORIG 0x829f7c5：
// 依序 CREATE_* 单例；Neof_shutdownGlobalInstances() 反序 DESTROY_*。
// ---------------------------------------------------------------------------
void Neof_startupGlobalInstances()
{
    CREATE_CEnvironment();
    CREATE_CGameManager();
    CREATE_CDataManager();
    CREATE_InterDispatcher();
    CREATE_EpollHandler();
    CREATE_PacketDispatcher();
    CREATE_Store();
    CREATE_TimerQueue();
    CREATE_TimerDispatcher();
    CREATE_GameWorld();
}