ActiveConManager::ActiveConManager()
    : queueRequestConnect(std::deque<ConInterface*>())
{
    bRequestInQueue = false;
    bConnectedInQueue = false;
