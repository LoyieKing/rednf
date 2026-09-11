// ORIG 0x081b0298：同 cancel 但不退款（tx->confirm() 仅自移除）。正向 if 同形。
void TransactionManager::confirm(unsigned int id)
{
    Transaction* tx = getTransaction(id);
    if (tx != 0)
    {
        tx->confirm();
        tx->getUser()->removeTransaction(tx);
        delete tx;
    }
}