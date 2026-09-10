void TransactionManager::confirm(unsigned int id)
{
    Transaction* tx = getTransaction(id);
    if (tx != 0)
    {
        tx->confirm();
        tx->getUser()->removeTransaction(tx);
        delete tx;
    }
