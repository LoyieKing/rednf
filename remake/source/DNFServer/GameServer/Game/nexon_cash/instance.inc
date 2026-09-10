TransactionManager& TransactionManager::instance()
{
    static TransactionManager i;
    return i;
