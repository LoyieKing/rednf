void UserBalance::addNewTransaction(Transaction* tx)
{
    printf("addNewTx: %p\n", tx);
    m_transactions.push_back(tx);
