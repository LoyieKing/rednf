void TransactionManager::cancel(unsigned int id)
{
    Transaction* tx = getTransaction(id);
    if (tx != 0)
    {
        tx->cancel();
        tx->getUser()->removeTransaction(tx);
        delete tx;
    }
