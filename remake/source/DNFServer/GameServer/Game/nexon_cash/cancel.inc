void Transaction::cancel()
{
    m_userBalance->balance(m_amount);
    TransactionManager::instance().removeTransaction(this);
