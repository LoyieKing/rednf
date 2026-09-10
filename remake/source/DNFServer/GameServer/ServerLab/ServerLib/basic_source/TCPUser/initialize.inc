void TCPUser::initialize()
{
    bDisconnected_ = false;
    bAboutToDisconnect_ = false;
    mbSending = false;
    DisLast_ = 0;
    m_PosByWouldBlock = 0;
    m_SizeByWouldBlock = 0;
    wouldBlockQueue.clear();
    pSession = 0;
    mbBindedSession = false;
    mbPassiveCloseSyncByWorker = false;
    mbActiveCloseSyncByWorker = false;
    pmWorkThread = 0;
    setLastAccessTime();
