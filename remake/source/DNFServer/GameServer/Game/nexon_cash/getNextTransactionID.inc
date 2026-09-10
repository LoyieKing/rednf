unsigned int TransactionManager::getNextTransactionID()
{
    return ++TransactionManager::tx_id_;
