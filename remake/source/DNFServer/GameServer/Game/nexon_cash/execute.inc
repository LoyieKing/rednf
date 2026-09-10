int Transaction::execute()
{
    if (m_userBalance->balance() < m_amount)
        return 3;
    m_userBalance->balance(-m_amount);
    TransactionManager::instance().addTransaction(this);
    return 0;
