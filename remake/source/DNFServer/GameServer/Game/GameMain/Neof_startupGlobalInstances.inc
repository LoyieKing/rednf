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
