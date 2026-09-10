void TransactionManager::removeTransaction(Transaction* tx)
{
    m_transactions.erase(tx->id());
