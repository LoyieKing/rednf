void Neof_shutdownGlobalInstances()
{
    DESTROY_GameWorld();
    DESTROY_TimerDispatcher();
    DESTROY_TimerQueue();
    DESTROY_Store();
    DESTROY_PacketDispatcher();
    DESTROY_EpollHandler();
    DESTROY_InterDispatcher();
    DESTROY_CDataManager();
    DESTROY_CGameManager();
    DESTROY_CEnvironment();
