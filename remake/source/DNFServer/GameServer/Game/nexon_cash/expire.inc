void UserBalance::expire()
{
    std::vector<Transaction*>::iterator it = m_transactions.begin();
    while (it != m_transactions.end())
    {
        if ((*it)->expired())
        {
            (*it)->cancel();
            it = m_transactions.erase(it);
        }
        else
        {
            ++it;
        }
    }
