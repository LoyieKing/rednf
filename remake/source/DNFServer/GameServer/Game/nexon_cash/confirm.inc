void Transaction::confirm()
{
    TransactionManager::instance().removeTransaction(this);
