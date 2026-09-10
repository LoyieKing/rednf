Transaction::Transaction(UserBalance* userBalance, int amount)
    : m_id(0), m_amount(amount), m_createdTime(0), m_userBalance(userBalance)
{
    m_id = TransactionManager::getNextTransactionID();
    m_createdTime = static_cast<int>(time(NULL));
