void TransactionManager::addTransaction(Transaction* tx)
{
    m_transactions.insert(std::make_pair(tx->id(), tx));
